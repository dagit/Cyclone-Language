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
 struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};struct Cyc_Core_Opt{
void*v;};struct _dyneither_ptr Cyc_Core_new_string(unsigned int);extern char Cyc_Core_Invalid_argument[
17];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};
extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{
char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{
char*tag;};struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{char*tag;};
extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*tag;struct
_dyneither_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct
Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char
Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*tag;};void
Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{
char*tag;};int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x);int Cyc_List_list_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);int Cyc_List_list_prefix(
int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);extern int
Cyc_PP_tex_output;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;void Cyc_PP_file_of_doc(
struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*f);struct _dyneither_ptr Cyc_PP_string_of_doc(
struct Cyc_PP_Doc*d,int w);struct Cyc_PP_Doc*Cyc_PP_nil_doc();struct Cyc_PP_Doc*Cyc_PP_blank_doc();
struct Cyc_PP_Doc*Cyc_PP_line_doc();struct Cyc_PP_Doc*Cyc_PP_text(struct
_dyneither_ptr s);struct Cyc_PP_Doc*Cyc_PP_textptr(struct _dyneither_ptr*p);struct
Cyc_PP_Doc*Cyc_PP_text_width(struct _dyneither_ptr s,int w);struct Cyc_PP_Doc*Cyc_PP_nest(
int k,struct Cyc_PP_Doc*d);struct Cyc_PP_Doc*Cyc_PP_cat(struct _dyneither_ptr);
struct Cyc_PP_Doc*Cyc_PP_seq(struct _dyneither_ptr sep,struct Cyc_List_List*l);
struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr
sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_seql(struct _dyneither_ptr sep,
struct Cyc_List_List*l0);struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(
void*),struct _dyneither_ptr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_group(
struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,
struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_egroup(struct _dyneither_ptr start,
struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);struct
Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int
line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};struct Cyc_Position_Segment;struct Cyc_Position_Lex_struct{int tag;};
struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{int tag;}
;struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];
struct Cyc_Position_Nocontext_struct{char*tag;};struct _union_Nmspace_Rel_n{int tag;
struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace
Cyc_Absyn_Rel_n(struct Cyc_List_List*);union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(
struct Cyc_List_List*);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct
_dyneither_ptr*f2;};enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract
 = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register
 = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int
real_const;struct Cyc_Position_Segment*loc;};enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz
 = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz
 = 4};enum Cyc_Absyn_Kind{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind
 = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_UniqueRgnKind  = 4,Cyc_Absyn_TopRgnKind  = 5,
Cyc_Absyn_EffKind  = 6,Cyc_Absyn_IntKind  = 7};enum Cyc_Absyn_Sign{Cyc_Absyn_Signed
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
int tag;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*
rgn_loc;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;
union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*
zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;
struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_struct{int tag;
void*f1;};struct Cyc_Absyn_Thin_ptrqual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_struct{
int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_struct{
int tag;};struct Cyc_Absyn_Notnull_ptrqual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_struct{
int tag;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual
tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;struct
Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct
Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;
struct _tuple0*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{
union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct
_tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};
struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct
_union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{
struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr
KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct
Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Position_Segment*
zt_loc;};struct Cyc_Absyn_VoidType_struct{int tag;};struct Cyc_Absyn_Evar_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;}
;struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_struct{
int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_struct{
int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_struct{
int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_struct{int tag;enum 
Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_struct{int
tag;};struct Cyc_Absyn_DoubleType_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{
int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_struct{int tag;struct
Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct
Cyc_Absyn_AnonAggrType_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple0*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void**f4;};struct Cyc_Absyn_ValueofType_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};
struct Cyc_Absyn_HeapRgn_struct{int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;
};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};
struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;};struct Cyc_Absyn_WithTypes_struct{int tag;struct Cyc_List_List*f1;int f2;struct
Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};enum Cyc_Absyn_Format_Type{
Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_struct{
int tag;int f1;};struct Cyc_Absyn_Stdcall_att_struct{int tag;};struct Cyc_Absyn_Cdecl_att_struct{
int tag;};struct Cyc_Absyn_Fastcall_att_struct{int tag;};struct Cyc_Absyn_Noreturn_att_struct{
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
_tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct
_tuple3 val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct
_union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign
f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum 
Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6
val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr val;};struct
_union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct
_union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;};enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times
 = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,
Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte
 = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor
 = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,
Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc
 = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{
int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
struct Cyc_Absyn_StructField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{
int tag;unsigned int f1;};enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,
Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion
 = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**
elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_struct{
int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple0*
f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple0*f1;};struct
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
int tag;struct Cyc_List_List*f1;};struct _tuple7{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple7*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
struct Cyc_Absyn_AnonEnum_e_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*
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
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple8{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple8 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple8 f2;struct _tuple8 f3;struct Cyc_Absyn_Stmt*f4;}
;struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple8 f2;
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
int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};extern struct Cyc_Absyn_Wild_p_struct Cyc_Absyn_Wild_p_val;
struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*
pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;};struct Cyc_Absyn_Global_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;
struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;
void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{
struct _tuple0*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum 
Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct
_tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;
};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct
Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual
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
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_compress_kb(
void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
extern void*Cyc_Absyn_bounds_one;struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);struct _dyneither_ptr
Cyc_Absyn_attribute2string(void*);struct _tuple9{enum Cyc_Absyn_AggrKind f1;struct
_tuple0*f2;};struct _tuple9 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
struct Cyc_Buffer_t;unsigned long Cyc_strlen(struct _dyneither_ptr s);int Cyc_strptrcmp(
struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct _dyneither_ptr Cyc_strconcat(
struct _dyneither_ptr,struct _dyneither_ptr);struct _dyneither_ptr Cyc_str_sepstr(
struct Cyc_List_List*,struct _dyneither_ptr);struct _tuple10{unsigned int f1;int f2;}
;struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{
char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{
char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};
struct _tuple11{void*f1;void*f2;};struct _tuple11*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple11*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_NotLoop_j_struct{int tag;};struct Cyc_Tcenv_CaseEnd_j_struct{
int tag;};struct Cyc_Tcenv_FnEnd_j_struct{int tag;};struct Cyc_Tcenv_Stmt_j_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void*Cyc_Tcutil_compress(
void*t);int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*);struct Cyc_Absynpp_Params{int expand_typedefs: 1;int
qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};extern int Cyc_Absynpp_print_for_cycdoc;
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params
Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params
Cyc_Absynpp_tc_params_r;void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,
struct Cyc___cycFILE*f);struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*
d);struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_typ2cstring(
void*);struct _dyneither_ptr Cyc_Absynpp_kind2string(enum Cyc_Absyn_Kind);struct
_dyneither_ptr Cyc_Absynpp_kindbound2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct _dyneither_ptr
Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);struct _dyneither_ptr Cyc_Absynpp_prim2string(
enum Cyc_Absyn_Primop p);struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*
p);struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc);int Cyc_Absynpp_is_anon_aggrtype(
void*t);extern struct _dyneither_ptr Cyc_Absynpp_cyc_string;extern struct
_dyneither_ptr*Cyc_Absynpp_cyc_stringptr;int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_char_escape(char);struct _dyneither_ptr Cyc_Absynpp_string_escape(
struct _dyneither_ptr);struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop
p);int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);struct _tuple12{struct
_dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct _tuple7*Cyc_Absynpp_arg_mk_opt(
struct _tuple12*arg);struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*
f3;};struct _tuple13 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual
tq,void*t);enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c
 = 1};extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;struct _tuple14{
struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(
struct _tuple14*dp);struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*
cs);struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope);
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*,int expstmt);struct
Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e);struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int
inprec,struct Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct
_tuple0*);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*);struct
Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst);struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(
enum Cyc_Absyn_Primop);struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum 
Cyc_Absyn_Primop p,struct Cyc_List_List*es);struct _tuple15{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple15*de);
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_Core_Opt*
dopt);struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*effopt,
struct Cyc_List_List*rgn_po);struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct
Cyc_List_List*fs);struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*
vd);static int Cyc_Absynpp_expand_typedefs;static int Cyc_Absynpp_qvar_to_Cids;
static char _tmp0[4]="Cyc";struct _dyneither_ptr Cyc_Absynpp_cyc_string={_tmp0,_tmp0,
_tmp0 + 4};struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr=& Cyc_Absynpp_cyc_string;
static int Cyc_Absynpp_add_cyc_prefix;static int Cyc_Absynpp_to_VC;static int Cyc_Absynpp_decls_first;
static int Cyc_Absynpp_rewrite_temp_tvars;static int Cyc_Absynpp_print_all_tvars;
static int Cyc_Absynpp_print_all_kinds;static int Cyc_Absynpp_print_all_effects;
static int Cyc_Absynpp_print_using_stmts;static int Cyc_Absynpp_print_externC_stmts;
static int Cyc_Absynpp_print_full_evars;static int Cyc_Absynpp_generate_line_directives;
static int Cyc_Absynpp_use_curr_namespace;static int Cyc_Absynpp_print_zeroterm;
static struct Cyc_List_List*Cyc_Absynpp_curr_namespace=0;int Cyc_Absynpp_print_for_cycdoc=
0;struct Cyc_Absynpp_Params;void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*
fs);void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs){Cyc_Absynpp_expand_typedefs=
fs->expand_typedefs;Cyc_Absynpp_qvar_to_Cids=fs->qvar_to_Cids;Cyc_Absynpp_add_cyc_prefix=
fs->add_cyc_prefix;Cyc_Absynpp_to_VC=fs->to_VC;Cyc_Absynpp_decls_first=fs->decls_first;
Cyc_Absynpp_rewrite_temp_tvars=fs->rewrite_temp_tvars;Cyc_Absynpp_print_all_tvars=
fs->print_all_tvars;Cyc_Absynpp_print_all_kinds=fs->print_all_kinds;Cyc_Absynpp_print_all_effects=
fs->print_all_effects;Cyc_Absynpp_print_using_stmts=fs->print_using_stmts;Cyc_Absynpp_print_externC_stmts=
fs->print_externC_stmts;Cyc_Absynpp_print_full_evars=fs->print_full_evars;Cyc_Absynpp_print_zeroterm=
fs->print_zeroterm;Cyc_Absynpp_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_use_curr_namespace=fs->use_curr_namespace;Cyc_Absynpp_curr_namespace=
fs->curr_namespace;}struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={0,0,0,0,0,
1,0,0,0,1,1,0,1,0,1,0};struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={1,0,0,
0,0,1,0,0,1,1,1,0,1,0,1,0};struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={1,1,1,
0,1,0,0,0,0,0,0,0,0,0,0,0};struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={0,0,
0,0,0,0,0,0,0,1,1,0,1,0,0,0};static void Cyc_Absynpp_curr_namespace_add(struct
_dyneither_ptr*v);static void Cyc_Absynpp_curr_namespace_add(struct _dyneither_ptr*
v){struct Cyc_List_List*_tmp5DE;Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,((
_tmp5DE=_cycalloc(sizeof(*_tmp5DE)),((_tmp5DE->hd=v,((_tmp5DE->tl=0,_tmp5DE)))))));}
static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l);static void Cyc_Absynpp_suppr_last(
struct Cyc_List_List**l){if(((struct Cyc_List_List*)_check_null(*l))->tl == 0)*l=0;
else{Cyc_Absynpp_suppr_last(&((struct Cyc_List_List*)_check_null(*l))->tl);}}
static void Cyc_Absynpp_curr_namespace_drop();static void Cyc_Absynpp_curr_namespace_drop(){((
void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
struct _dyneither_ptr Cyc_Absynpp_char_escape(char c);struct _dyneither_ptr Cyc_Absynpp_char_escape(
char c){switch(c){case '\a': _LL0: {const char*_tmp5DF;return(_tmp5DF="\\a",
_tag_dyneither(_tmp5DF,sizeof(char),3));}case '\b': _LL1: {const char*_tmp5E0;
return(_tmp5E0="\\b",_tag_dyneither(_tmp5E0,sizeof(char),3));}case '\f': _LL2: {
const char*_tmp5E1;return(_tmp5E1="\\f",_tag_dyneither(_tmp5E1,sizeof(char),3));}
case '\n': _LL3: {const char*_tmp5E2;return(_tmp5E2="\\n",_tag_dyneither(_tmp5E2,
sizeof(char),3));}case '\r': _LL4: {const char*_tmp5E3;return(_tmp5E3="\\r",
_tag_dyneither(_tmp5E3,sizeof(char),3));}case '\t': _LL5: {const char*_tmp5E4;
return(_tmp5E4="\\t",_tag_dyneither(_tmp5E4,sizeof(char),3));}case '\v': _LL6: {
const char*_tmp5E5;return(_tmp5E5="\\v",_tag_dyneither(_tmp5E5,sizeof(char),3));}
case '\\': _LL7: {const char*_tmp5E6;return(_tmp5E6="\\\\",_tag_dyneither(_tmp5E6,
sizeof(char),3));}case '"': _LL8: {const char*_tmp5E7;return(_tmp5E7="\"",
_tag_dyneither(_tmp5E7,sizeof(char),2));}case '\'': _LL9: {const char*_tmp5E8;
return(_tmp5E8="\\'",_tag_dyneither(_tmp5E8,sizeof(char),3));}default: _LLA: if(c
>= ' '  && c <= '~'){struct _dyneither_ptr _tmpC=Cyc_Core_new_string(2);{char _tmp5EB;
char _tmp5EA;struct _dyneither_ptr _tmp5E9;(_tmp5E9=_dyneither_ptr_plus(_tmpC,
sizeof(char),0),((_tmp5EA=*((char*)_check_dyneither_subscript(_tmp5E9,sizeof(
char),0)),((_tmp5EB=c,((_get_dyneither_size(_tmp5E9,sizeof(char))== 1  && (
_tmp5EA == '\000'  && _tmp5EB != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5E9.curr)=
_tmp5EB)))))));}return(struct _dyneither_ptr)_tmpC;}else{struct _dyneither_ptr
_tmp10=Cyc_Core_new_string(5);int j=0;{char _tmp5EE;char _tmp5ED;struct
_dyneither_ptr _tmp5EC;(_tmp5EC=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((
_tmp5ED=*((char*)_check_dyneither_subscript(_tmp5EC,sizeof(char),0)),((_tmp5EE='\\',((
_get_dyneither_size(_tmp5EC,sizeof(char))== 1  && (_tmp5ED == '\000'  && _tmp5EE != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp5EC.curr)=_tmp5EE)))))));}{char _tmp5F1;char
_tmp5F0;struct _dyneither_ptr _tmp5EF;(_tmp5EF=_dyneither_ptr_plus(_tmp10,sizeof(
char),j ++),((_tmp5F0=*((char*)_check_dyneither_subscript(_tmp5EF,sizeof(char),0)),((
_tmp5F1=(char)('0' + ((unsigned char)c >> 6 & 3)),((_get_dyneither_size(_tmp5EF,
sizeof(char))== 1  && (_tmp5F0 == '\000'  && _tmp5F1 != '\000')?_throw_arraybounds():
1,*((char*)_tmp5EF.curr)=_tmp5F1)))))));}{char _tmp5F4;char _tmp5F3;struct
_dyneither_ptr _tmp5F2;(_tmp5F2=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((
_tmp5F3=*((char*)_check_dyneither_subscript(_tmp5F2,sizeof(char),0)),((_tmp5F4=(
char)('0' + (c >> 3 & 7)),((_get_dyneither_size(_tmp5F2,sizeof(char))== 1  && (
_tmp5F3 == '\000'  && _tmp5F4 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5F2.curr)=
_tmp5F4)))))));}{char _tmp5F7;char _tmp5F6;struct _dyneither_ptr _tmp5F5;(_tmp5F5=
_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp5F6=*((char*)
_check_dyneither_subscript(_tmp5F5,sizeof(char),0)),((_tmp5F7=(char)('0' + (c & 7)),((
_get_dyneither_size(_tmp5F5,sizeof(char))== 1  && (_tmp5F6 == '\000'  && _tmp5F7 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp5F5.curr)=_tmp5F7)))))));}return(struct
_dyneither_ptr)_tmp10;}}}static int Cyc_Absynpp_special(struct _dyneither_ptr s);
static int Cyc_Absynpp_special(struct _dyneither_ptr s){int sz=(int)(
_get_dyneither_size(s,sizeof(char))- 1);{int i=0;for(0;i < sz;++ i){char c=*((const
char*)_check_dyneither_subscript(s,sizeof(char),i));if(((c <= ' '  || c >= '~') || 
c == '"') || c == '\\')return 1;}}return 0;}struct _dyneither_ptr Cyc_Absynpp_string_escape(
struct _dyneither_ptr s);struct _dyneither_ptr Cyc_Absynpp_string_escape(struct
_dyneither_ptr s){if(!Cyc_Absynpp_special(s))return s;{int n=(int)(
_get_dyneither_size(s,sizeof(char))- 1);if(n > 0  && *((const char*)
_check_dyneither_subscript(s,sizeof(char),n))== '\000')-- n;{int len=0;{int i=0;
for(0;i <= n;++ i){char _tmp1D=*((const char*)_check_dyneither_subscript(s,sizeof(
char),i));_LLD: if(_tmp1D != '\a')goto _LLF;_LLE: goto _LL10;_LLF: if(_tmp1D != '\b')
goto _LL11;_LL10: goto _LL12;_LL11: if(_tmp1D != '\f')goto _LL13;_LL12: goto _LL14;_LL13:
if(_tmp1D != '\n')goto _LL15;_LL14: goto _LL16;_LL15: if(_tmp1D != '\r')goto _LL17;
_LL16: goto _LL18;_LL17: if(_tmp1D != '\t')goto _LL19;_LL18: goto _LL1A;_LL19: if(_tmp1D
!= '\v')goto _LL1B;_LL1A: goto _LL1C;_LL1B: if(_tmp1D != '\\')goto _LL1D;_LL1C: goto
_LL1E;_LL1D: if(_tmp1D != '"')goto _LL1F;_LL1E: len +=2;goto _LLC;_LL1F:;_LL20: if(
_tmp1D >= ' '  && _tmp1D <= '~')++ len;else{len +=4;}goto _LLC;_LLC:;}}{struct
_dyneither_ptr t=Cyc_Core_new_string((unsigned int)(len + 1));int j=0;{int i=0;for(0;
i <= n;++ i){char _tmp1E=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
_LL22: if(_tmp1E != '\a')goto _LL24;_LL23:{char _tmp5FA;char _tmp5F9;struct
_dyneither_ptr _tmp5F8;(_tmp5F8=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5F9=*((
char*)_check_dyneither_subscript(_tmp5F8,sizeof(char),0)),((_tmp5FA='\\',((
_get_dyneither_size(_tmp5F8,sizeof(char))== 1  && (_tmp5F9 == '\000'  && _tmp5FA != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp5F8.curr)=_tmp5FA)))))));}{char _tmp5FD;char
_tmp5FC;struct _dyneither_ptr _tmp5FB;(_tmp5FB=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp5FC=*((char*)_check_dyneither_subscript(_tmp5FB,sizeof(char),0)),((
_tmp5FD='a',((_get_dyneither_size(_tmp5FB,sizeof(char))== 1  && (_tmp5FC == '\000'
 && _tmp5FD != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5FB.curr)=_tmp5FD)))))));}
goto _LL21;_LL24: if(_tmp1E != '\b')goto _LL26;_LL25:{char _tmp600;char _tmp5FF;struct
_dyneither_ptr _tmp5FE;(_tmp5FE=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5FF=*((
char*)_check_dyneither_subscript(_tmp5FE,sizeof(char),0)),((_tmp600='\\',((
_get_dyneither_size(_tmp5FE,sizeof(char))== 1  && (_tmp5FF == '\000'  && _tmp600 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp5FE.curr)=_tmp600)))))));}{char _tmp603;char
_tmp602;struct _dyneither_ptr _tmp601;(_tmp601=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp602=*((char*)_check_dyneither_subscript(_tmp601,sizeof(char),0)),((
_tmp603='b',((_get_dyneither_size(_tmp601,sizeof(char))== 1  && (_tmp602 == '\000'
 && _tmp603 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp601.curr)=_tmp603)))))));}
goto _LL21;_LL26: if(_tmp1E != '\f')goto _LL28;_LL27:{char _tmp606;char _tmp605;struct
_dyneither_ptr _tmp604;(_tmp604=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp605=*((
char*)_check_dyneither_subscript(_tmp604,sizeof(char),0)),((_tmp606='\\',((
_get_dyneither_size(_tmp604,sizeof(char))== 1  && (_tmp605 == '\000'  && _tmp606 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp604.curr)=_tmp606)))))));}{char _tmp609;char
_tmp608;struct _dyneither_ptr _tmp607;(_tmp607=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp608=*((char*)_check_dyneither_subscript(_tmp607,sizeof(char),0)),((
_tmp609='f',((_get_dyneither_size(_tmp607,sizeof(char))== 1  && (_tmp608 == '\000'
 && _tmp609 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp607.curr)=_tmp609)))))));}
goto _LL21;_LL28: if(_tmp1E != '\n')goto _LL2A;_LL29:{char _tmp60C;char _tmp60B;struct
_dyneither_ptr _tmp60A;(_tmp60A=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp60B=*((
char*)_check_dyneither_subscript(_tmp60A,sizeof(char),0)),((_tmp60C='\\',((
_get_dyneither_size(_tmp60A,sizeof(char))== 1  && (_tmp60B == '\000'  && _tmp60C != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp60A.curr)=_tmp60C)))))));}{char _tmp60F;char
_tmp60E;struct _dyneither_ptr _tmp60D;(_tmp60D=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp60E=*((char*)_check_dyneither_subscript(_tmp60D,sizeof(char),0)),((
_tmp60F='n',((_get_dyneither_size(_tmp60D,sizeof(char))== 1  && (_tmp60E == '\000'
 && _tmp60F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp60D.curr)=_tmp60F)))))));}
goto _LL21;_LL2A: if(_tmp1E != '\r')goto _LL2C;_LL2B:{char _tmp612;char _tmp611;struct
_dyneither_ptr _tmp610;(_tmp610=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp611=*((
char*)_check_dyneither_subscript(_tmp610,sizeof(char),0)),((_tmp612='\\',((
_get_dyneither_size(_tmp610,sizeof(char))== 1  && (_tmp611 == '\000'  && _tmp612 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp610.curr)=_tmp612)))))));}{char _tmp615;char
_tmp614;struct _dyneither_ptr _tmp613;(_tmp613=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp614=*((char*)_check_dyneither_subscript(_tmp613,sizeof(char),0)),((
_tmp615='r',((_get_dyneither_size(_tmp613,sizeof(char))== 1  && (_tmp614 == '\000'
 && _tmp615 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp613.curr)=_tmp615)))))));}
goto _LL21;_LL2C: if(_tmp1E != '\t')goto _LL2E;_LL2D:{char _tmp618;char _tmp617;struct
_dyneither_ptr _tmp616;(_tmp616=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp617=*((
char*)_check_dyneither_subscript(_tmp616,sizeof(char),0)),((_tmp618='\\',((
_get_dyneither_size(_tmp616,sizeof(char))== 1  && (_tmp617 == '\000'  && _tmp618 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp616.curr)=_tmp618)))))));}{char _tmp61B;char
_tmp61A;struct _dyneither_ptr _tmp619;(_tmp619=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp61A=*((char*)_check_dyneither_subscript(_tmp619,sizeof(char),0)),((
_tmp61B='t',((_get_dyneither_size(_tmp619,sizeof(char))== 1  && (_tmp61A == '\000'
 && _tmp61B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp619.curr)=_tmp61B)))))));}
goto _LL21;_LL2E: if(_tmp1E != '\v')goto _LL30;_LL2F:{char _tmp61E;char _tmp61D;struct
_dyneither_ptr _tmp61C;(_tmp61C=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp61D=*((
char*)_check_dyneither_subscript(_tmp61C,sizeof(char),0)),((_tmp61E='\\',((
_get_dyneither_size(_tmp61C,sizeof(char))== 1  && (_tmp61D == '\000'  && _tmp61E != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp61C.curr)=_tmp61E)))))));}{char _tmp621;char
_tmp620;struct _dyneither_ptr _tmp61F;(_tmp61F=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp620=*((char*)_check_dyneither_subscript(_tmp61F,sizeof(char),0)),((
_tmp621='v',((_get_dyneither_size(_tmp61F,sizeof(char))== 1  && (_tmp620 == '\000'
 && _tmp621 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp61F.curr)=_tmp621)))))));}
goto _LL21;_LL30: if(_tmp1E != '\\')goto _LL32;_LL31:{char _tmp624;char _tmp623;struct
_dyneither_ptr _tmp622;(_tmp622=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp623=*((
char*)_check_dyneither_subscript(_tmp622,sizeof(char),0)),((_tmp624='\\',((
_get_dyneither_size(_tmp622,sizeof(char))== 1  && (_tmp623 == '\000'  && _tmp624 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp622.curr)=_tmp624)))))));}{char _tmp627;char
_tmp626;struct _dyneither_ptr _tmp625;(_tmp625=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp626=*((char*)_check_dyneither_subscript(_tmp625,sizeof(char),0)),((
_tmp627='\\',((_get_dyneither_size(_tmp625,sizeof(char))== 1  && (_tmp626 == '\000'
 && _tmp627 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp625.curr)=_tmp627)))))));}
goto _LL21;_LL32: if(_tmp1E != '"')goto _LL34;_LL33:{char _tmp62A;char _tmp629;struct
_dyneither_ptr _tmp628;(_tmp628=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp629=*((
char*)_check_dyneither_subscript(_tmp628,sizeof(char),0)),((_tmp62A='\\',((
_get_dyneither_size(_tmp628,sizeof(char))== 1  && (_tmp629 == '\000'  && _tmp62A != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp628.curr)=_tmp62A)))))));}{char _tmp62D;char
_tmp62C;struct _dyneither_ptr _tmp62B;(_tmp62B=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp62C=*((char*)_check_dyneither_subscript(_tmp62B,sizeof(char),0)),((
_tmp62D='"',((_get_dyneither_size(_tmp62B,sizeof(char))== 1  && (_tmp62C == '\000'
 && _tmp62D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp62B.curr)=_tmp62D)))))));}
goto _LL21;_LL34:;_LL35: if(_tmp1E >= ' '  && _tmp1E <= '~'){char _tmp630;char _tmp62F;
struct _dyneither_ptr _tmp62E;(_tmp62E=_dyneither_ptr_plus(t,sizeof(char),j ++),((
_tmp62F=*((char*)_check_dyneither_subscript(_tmp62E,sizeof(char),0)),((_tmp630=
_tmp1E,((_get_dyneither_size(_tmp62E,sizeof(char))== 1  && (_tmp62F == '\000'  && 
_tmp630 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp62E.curr)=_tmp630)))))));}
else{{char _tmp633;char _tmp632;struct _dyneither_ptr _tmp631;(_tmp631=
_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp632=*((char*)
_check_dyneither_subscript(_tmp631,sizeof(char),0)),((_tmp633='\\',((
_get_dyneither_size(_tmp631,sizeof(char))== 1  && (_tmp632 == '\000'  && _tmp633 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp631.curr)=_tmp633)))))));}{char _tmp636;char
_tmp635;struct _dyneither_ptr _tmp634;(_tmp634=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp635=*((char*)_check_dyneither_subscript(_tmp634,sizeof(char),0)),((
_tmp636=(char)('0' + (_tmp1E >> 6 & 7)),((_get_dyneither_size(_tmp634,sizeof(char))
== 1  && (_tmp635 == '\000'  && _tmp636 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp634.curr)=_tmp636)))))));}{char _tmp639;char _tmp638;struct _dyneither_ptr
_tmp637;(_tmp637=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp638=*((char*)
_check_dyneither_subscript(_tmp637,sizeof(char),0)),((_tmp639=(char)('0' + (
_tmp1E >> 3 & 7)),((_get_dyneither_size(_tmp637,sizeof(char))== 1  && (_tmp638 == '\000'
 && _tmp639 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp637.curr)=_tmp639)))))));}{
char _tmp63C;char _tmp63B;struct _dyneither_ptr _tmp63A;(_tmp63A=_dyneither_ptr_plus(
t,sizeof(char),j ++),((_tmp63B=*((char*)_check_dyneither_subscript(_tmp63A,
sizeof(char),0)),((_tmp63C=(char)('0' + (_tmp1E & 7)),((_get_dyneither_size(
_tmp63A,sizeof(char))== 1  && (_tmp63B == '\000'  && _tmp63C != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp63A.curr)=_tmp63C)))))));};}goto _LL21;_LL21:;}}
return(struct _dyneither_ptr)t;};};};}static char _tmp64[9]="restrict";static struct
_dyneither_ptr Cyc_Absynpp_restrict_string={_tmp64,_tmp64,_tmp64 + 9};static char
_tmp65[9]="volatile";static struct _dyneither_ptr Cyc_Absynpp_volatile_string={
_tmp65,_tmp65,_tmp65 + 9};static char _tmp66[6]="const";static struct _dyneither_ptr
Cyc_Absynpp_const_str={_tmp66,_tmp66,_tmp66 + 6};static struct _dyneither_ptr*Cyc_Absynpp_restrict_sp=&
Cyc_Absynpp_restrict_string;static struct _dyneither_ptr*Cyc_Absynpp_volatile_sp=&
Cyc_Absynpp_volatile_string;static struct _dyneither_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq);struct Cyc_PP_Doc*
Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){struct Cyc_List_List*l=0;if(tq.q_restrict){
struct Cyc_List_List*_tmp63D;l=((_tmp63D=_cycalloc(sizeof(*_tmp63D)),((_tmp63D->hd=
Cyc_Absynpp_restrict_sp,((_tmp63D->tl=l,_tmp63D))))));}if(tq.q_volatile){struct
Cyc_List_List*_tmp63E;l=((_tmp63E=_cycalloc(sizeof(*_tmp63E)),((_tmp63E->hd=Cyc_Absynpp_volatile_sp,((
_tmp63E->tl=l,_tmp63E))))));}if(tq.print_const){struct Cyc_List_List*_tmp63F;l=((
_tmp63F=_cycalloc(sizeof(*_tmp63F)),((_tmp63F->hd=Cyc_Absynpp_const_sp,((_tmp63F->tl=
l,_tmp63F))))));}{const char*_tmp642;const char*_tmp641;const char*_tmp640;return
Cyc_PP_egroup(((_tmp640="",_tag_dyneither(_tmp640,sizeof(char),1))),((_tmp641=" ",
_tag_dyneither(_tmp641,sizeof(char),2))),((_tmp642=" ",_tag_dyneither(_tmp642,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));};}
struct _dyneither_ptr Cyc_Absynpp_kind2string(enum Cyc_Absyn_Kind k);struct
_dyneither_ptr Cyc_Absynpp_kind2string(enum Cyc_Absyn_Kind k){switch(k){case Cyc_Absyn_AnyKind:
_LL36: {const char*_tmp643;return(_tmp643="A",_tag_dyneither(_tmp643,sizeof(char),
2));}case Cyc_Absyn_MemKind: _LL37: {const char*_tmp644;return(_tmp644="M",
_tag_dyneither(_tmp644,sizeof(char),2));}case Cyc_Absyn_BoxKind: _LL38: {const char*
_tmp645;return(_tmp645="B",_tag_dyneither(_tmp645,sizeof(char),2));}case Cyc_Absyn_RgnKind:
_LL39: {const char*_tmp646;return(_tmp646="R",_tag_dyneither(_tmp646,sizeof(char),
2));}case Cyc_Absyn_UniqueRgnKind: _LL3A: {const char*_tmp647;return(_tmp647="UR",
_tag_dyneither(_tmp647,sizeof(char),3));}case Cyc_Absyn_TopRgnKind: _LL3B: {const
char*_tmp648;return(_tmp648="TR",_tag_dyneither(_tmp648,sizeof(char),3));}case
Cyc_Absyn_EffKind: _LL3C: {const char*_tmp649;return(_tmp649="E",_tag_dyneither(
_tmp649,sizeof(char),2));}case Cyc_Absyn_IntKind: _LL3D: {const char*_tmp64A;return(
_tmp64A="I",_tag_dyneither(_tmp64A,sizeof(char),2));}}}struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(
enum Cyc_Absyn_Kind k);struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(enum Cyc_Absyn_Kind k){
return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}struct _dyneither_ptr Cyc_Absynpp_kindbound2string(
void*c);struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){void*_tmp75=Cyc_Absyn_compress_kb(
c);enum Cyc_Absyn_Kind _tmp77;enum Cyc_Absyn_Kind _tmp7A;_LL40: {struct Cyc_Absyn_Eq_kb_struct*
_tmp76=(struct Cyc_Absyn_Eq_kb_struct*)_tmp75;if(_tmp76->tag != 0)goto _LL42;else{
_tmp77=_tmp76->f1;}}_LL41: return Cyc_Absynpp_kind2string(_tmp77);_LL42: {struct
Cyc_Absyn_Unknown_kb_struct*_tmp78=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp75;
if(_tmp78->tag != 1)goto _LL44;}_LL43: if(Cyc_PP_tex_output){const char*_tmp64B;
return(_tmp64B="{?}",_tag_dyneither(_tmp64B,sizeof(char),4));}else{const char*
_tmp64C;return(_tmp64C="?",_tag_dyneither(_tmp64C,sizeof(char),2));}_LL44: {
struct Cyc_Absyn_Less_kb_struct*_tmp79=(struct Cyc_Absyn_Less_kb_struct*)_tmp75;
if(_tmp79->tag != 2)goto _LL3F;else{_tmp7A=_tmp79->f2;}}_LL45: return Cyc_Absynpp_kind2string(
_tmp7A);_LL3F:;}struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c);struct Cyc_PP_Doc*
Cyc_Absynpp_kindbound2doc(void*c){void*_tmp7D=Cyc_Absyn_compress_kb(c);enum Cyc_Absyn_Kind
_tmp7F;enum Cyc_Absyn_Kind _tmp82;_LL47: {struct Cyc_Absyn_Eq_kb_struct*_tmp7E=(
struct Cyc_Absyn_Eq_kb_struct*)_tmp7D;if(_tmp7E->tag != 0)goto _LL49;else{_tmp7F=
_tmp7E->f1;}}_LL48: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp7F));_LL49: {
struct Cyc_Absyn_Unknown_kb_struct*_tmp80=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp7D;if(_tmp80->tag != 1)goto _LL4B;}_LL4A: if(Cyc_PP_tex_output){const char*
_tmp64D;return Cyc_PP_text_width(((_tmp64D="{?}",_tag_dyneither(_tmp64D,sizeof(
char),4))),1);}else{const char*_tmp64E;return Cyc_PP_text(((_tmp64E="?",
_tag_dyneither(_tmp64E,sizeof(char),2))));}_LL4B: {struct Cyc_Absyn_Less_kb_struct*
_tmp81=(struct Cyc_Absyn_Less_kb_struct*)_tmp7D;if(_tmp81->tag != 2)goto _LL46;
else{_tmp82=_tmp81->f2;}}_LL4C: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp82));
_LL46:;}struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts);struct Cyc_PP_Doc*
Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){const char*_tmp651;const char*_tmp650;
const char*_tmp64F;return Cyc_PP_egroup(((_tmp64F="<",_tag_dyneither(_tmp64F,
sizeof(char),2))),((_tmp650=">",_tag_dyneither(_tmp650,sizeof(char),2))),((
_tmp651=",",_tag_dyneither(_tmp651,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,
ts));}struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv);struct Cyc_PP_Doc*
Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){void*_tmp88=Cyc_Absyn_compress_kb(
tv->kind);enum Cyc_Absyn_Kind _tmp8B;enum Cyc_Absyn_Kind _tmp8D;enum Cyc_Absyn_Kind
_tmp8F;_LL4E: {struct Cyc_Absyn_Unknown_kb_struct*_tmp89=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp88;if(_tmp89->tag != 1)goto _LL50;}_LL4F: goto _LL51;_LL50: {struct Cyc_Absyn_Eq_kb_struct*
_tmp8A=(struct Cyc_Absyn_Eq_kb_struct*)_tmp88;if(_tmp8A->tag != 0)goto _LL52;else{
_tmp8B=_tmp8A->f1;if(_tmp8B != Cyc_Absyn_BoxKind)goto _LL52;}}_LL51: return Cyc_PP_textptr(
tv->name);_LL52: {struct Cyc_Absyn_Less_kb_struct*_tmp8C=(struct Cyc_Absyn_Less_kb_struct*)
_tmp88;if(_tmp8C->tag != 2)goto _LL54;else{_tmp8D=_tmp8C->f2;}}_LL53: _tmp8F=_tmp8D;
goto _LL55;_LL54: {struct Cyc_Absyn_Eq_kb_struct*_tmp8E=(struct Cyc_Absyn_Eq_kb_struct*)
_tmp88;if(_tmp8E->tag != 0)goto _LL4D;else{_tmp8F=_tmp8E->f1;}}_LL55: {const char*
_tmp654;struct Cyc_PP_Doc*_tmp653[3];return(_tmp653[2]=Cyc_Absynpp_kind2doc(
_tmp8F),((_tmp653[1]=Cyc_PP_text(((_tmp654="::",_tag_dyneither(_tmp654,sizeof(
char),3)))),((_tmp653[0]=Cyc_PP_textptr(tv->name),Cyc_PP_cat(_tag_dyneither(
_tmp653,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL4D:;}struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(
struct Cyc_List_List*tvs);struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*
tvs){const char*_tmp657;const char*_tmp656;const char*_tmp655;return Cyc_PP_egroup(((
_tmp655="<",_tag_dyneither(_tmp655,sizeof(char),2))),((_tmp656=">",
_tag_dyneither(_tmp656,sizeof(char),2))),((_tmp657=",",_tag_dyneither(_tmp657,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));}static struct
_dyneither_ptr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv);static struct
_dyneither_ptr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv){return tv->name;}
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs);struct Cyc_PP_Doc*
Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);{const char*_tmp65A;const char*_tmp659;const char*
_tmp658;return Cyc_PP_egroup(((_tmp658="<",_tag_dyneither(_tmp658,sizeof(char),2))),((
_tmp659=">",_tag_dyneither(_tmp659,sizeof(char),2))),((_tmp65A=",",
_tag_dyneither(_tmp65A,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((
struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,tvs)));};}struct _tuple16{
struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct
_tuple16*t);struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple16*t){return Cyc_Absynpp_tqtd2doc((*
t).f1,(*t).f2,0);}struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts);
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){const char*_tmp65D;
const char*_tmp65C;const char*_tmp65B;return Cyc_PP_group(((_tmp65B="(",
_tag_dyneither(_tmp65B,sizeof(char),2))),((_tmp65C=")",_tag_dyneither(_tmp65C,
sizeof(char),2))),((_tmp65D=",",_tag_dyneither(_tmp65D,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(
void*att);struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){void*_tmp9B=att;
_LL57: {struct Cyc_Absyn_Stdcall_att_struct*_tmp9C=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmp9B;if(_tmp9C->tag != 1)goto _LL59;}_LL58: goto _LL5A;_LL59: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp9D=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp9B;if(_tmp9D->tag != 2)goto _LL5B;}
_LL5A: goto _LL5C;_LL5B: {struct Cyc_Absyn_Fastcall_att_struct*_tmp9E=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp9B;if(_tmp9E->tag != 3)goto _LL5D;}_LL5C: return Cyc_PP_nil_doc();_LL5D:;_LL5E:
return Cyc_PP_text(Cyc_Absyn_attribute2string(att));_LL56:;}struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(
struct Cyc_List_List*atts);struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*
atts){for(0;atts != 0;atts=atts->tl){void*_tmp9F=(void*)atts->hd;_LL60: {struct
Cyc_Absyn_Stdcall_att_struct*_tmpA0=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp9F;
if(_tmpA0->tag != 1)goto _LL62;}_LL61: {const char*_tmp65E;return Cyc_PP_text(((
_tmp65E=" _stdcall ",_tag_dyneither(_tmp65E,sizeof(char),11))));}_LL62: {struct
Cyc_Absyn_Cdecl_att_struct*_tmpA1=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp9F;if(
_tmpA1->tag != 2)goto _LL64;}_LL63: {const char*_tmp65F;return Cyc_PP_text(((_tmp65F=" _cdecl ",
_tag_dyneither(_tmp65F,sizeof(char),9))));}_LL64: {struct Cyc_Absyn_Fastcall_att_struct*
_tmpA2=(struct Cyc_Absyn_Fastcall_att_struct*)_tmp9F;if(_tmpA2->tag != 3)goto _LL66;}
_LL65: {const char*_tmp660;return Cyc_PP_text(((_tmp660=" _fastcall ",
_tag_dyneither(_tmp660,sizeof(char),12))));}_LL66:;_LL67: goto _LL5F;_LL5F:;}
return Cyc_PP_nil_doc();}struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*
atts);struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){int
hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){void*
_tmpA6=(void*)atts2->hd;_LL69: {struct Cyc_Absyn_Stdcall_att_struct*_tmpA7=(
struct Cyc_Absyn_Stdcall_att_struct*)_tmpA6;if(_tmpA7->tag != 1)goto _LL6B;}_LL6A:
goto _LL6C;_LL6B: {struct Cyc_Absyn_Cdecl_att_struct*_tmpA8=(struct Cyc_Absyn_Cdecl_att_struct*)
_tmpA6;if(_tmpA8->tag != 2)goto _LL6D;}_LL6C: goto _LL6E;_LL6D: {struct Cyc_Absyn_Fastcall_att_struct*
_tmpA9=(struct Cyc_Absyn_Fastcall_att_struct*)_tmpA6;if(_tmpA9->tag != 3)goto _LL6F;}
_LL6E: goto _LL68;_LL6F:;_LL70: hasatt=1;goto _LL68;_LL68:;}}if(!hasatt)return Cyc_PP_nil_doc();{
const char*_tmp66B;const char*_tmp66A;const char*_tmp669;const char*_tmp668;const
char*_tmp667;struct Cyc_PP_Doc*_tmp666[3];return(_tmp666[2]=Cyc_PP_text(((_tmp667=")",
_tag_dyneither(_tmp667,sizeof(char),2)))),((_tmp666[1]=Cyc_PP_group(((_tmp66A="",
_tag_dyneither(_tmp66A,sizeof(char),1))),((_tmp669="",_tag_dyneither(_tmp669,
sizeof(char),1))),((_tmp668=" ",_tag_dyneither(_tmp668,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_noncallatt2doc,atts)),((_tmp666[0]=Cyc_PP_text(((_tmp66B=" __declspec(",
_tag_dyneither(_tmp66B,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp666,
sizeof(struct Cyc_PP_Doc*),3)))))));};}struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*
a);struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){return Cyc_PP_text(Cyc_Absyn_attribute2string(
a));}struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts);struct Cyc_PP_Doc*
Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){if(atts == 0)return Cyc_PP_nil_doc();
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL71: return Cyc_Absynpp_noncallconv2doc(
atts);case Cyc_Cyclone_Gcc_c: _LL72: {const char*_tmp674;const char*_tmp673;const
char*_tmp672;const char*_tmp671;struct Cyc_PP_Doc*_tmp670[2];return(_tmp670[1]=Cyc_PP_group(((
_tmp673="((",_tag_dyneither(_tmp673,sizeof(char),3))),((_tmp672="))",
_tag_dyneither(_tmp672,sizeof(char),3))),((_tmp671=",",_tag_dyneither(_tmp671,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts)),((_tmp670[0]=Cyc_PP_text(((
_tmp674=" __attribute__",_tag_dyneither(_tmp674,sizeof(char),15)))),Cyc_PP_cat(
_tag_dyneither(_tmp670,sizeof(struct Cyc_PP_Doc*),2)))));}}}int Cyc_Absynpp_next_is_pointer(
struct Cyc_List_List*tms);int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{void*_tmpB5=(void*)tms->hd;_LL75: {struct Cyc_Absyn_Pointer_mod_struct*
_tmpB6=(struct Cyc_Absyn_Pointer_mod_struct*)_tmpB5;if(_tmpB6->tag != 2)goto _LL77;}
_LL76: return 1;_LL77: {struct Cyc_Absyn_Attributes_mod_struct*_tmpB7=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpB5;if(_tmpB7->tag != 5)goto _LL79;}_LL78: switch(Cyc_Cyclone_c_compiler){case
Cyc_Cyclone_Gcc_c: _LL7B: return 0;default: _LL7C: return Cyc_Absynpp_next_is_pointer(
tms->tl);}_LL79:;_LL7A: return 0;_LL74:;};}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(
void*t);static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;static struct Cyc_PP_Doc*
Cyc_Absynpp_question();static struct Cyc_PP_Doc*Cyc_Absynpp_question(){if(!((
unsigned int)Cyc_Absynpp_cache_question)){if(Cyc_PP_tex_output){const char*
_tmp675;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((
_tmp675="{?}",_tag_dyneither(_tmp675,sizeof(char),4))),1);}else{const char*
_tmp676;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp676="?",
_tag_dyneither(_tmp676,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)_check_null(
Cyc_Absynpp_cache_question);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb();static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned int)Cyc_Absynpp_cache_lb)){if(Cyc_PP_tex_output){const char*
_tmp677;Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp677="{\\lb}",
_tag_dyneither(_tmp677,sizeof(char),6))),1);}else{const char*_tmp678;Cyc_Absynpp_cache_lb=(
struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp678="{",_tag_dyneither(_tmp678,sizeof(char),
2))));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}static struct
Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;static struct Cyc_PP_Doc*Cyc_Absynpp_rb();static
struct Cyc_PP_Doc*Cyc_Absynpp_rb(){if(!((unsigned int)Cyc_Absynpp_cache_rb)){if(
Cyc_PP_tex_output){const char*_tmp679;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((
_tmp679="{\\rb}",_tag_dyneither(_tmp679,sizeof(char),6))),1);}else{const char*
_tmp67A;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp67A="}",
_tag_dyneither(_tmp67A,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)_check_null(
Cyc_Absynpp_cache_rb);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;static
struct Cyc_PP_Doc*Cyc_Absynpp_dollar();static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){if(Cyc_PP_tex_output){const char*
_tmp67B;Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp67B="\\$",
_tag_dyneither(_tmp67B,sizeof(char),3))),1);}else{const char*_tmp67C;Cyc_Absynpp_cache_dollar=(
struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp67C="$",_tag_dyneither(_tmp67C,sizeof(char),
2))));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}struct
Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*
ss);struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*
ss){struct Cyc_PP_Doc*_tmp67D[3];return(_tmp67D[2]=Cyc_Absynpp_rb(),((_tmp67D[1]=
Cyc_PP_seq(sep,ss),((_tmp67D[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(
_tmp67D,sizeof(struct Cyc_PP_Doc*),3)))))));}static void Cyc_Absynpp_print_tms(
struct Cyc_List_List*tms);static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){void*_tmpC1=(void*)tms->hd;void*_tmpC5;struct Cyc_List_List*
_tmpC7;_LL7F: {struct Cyc_Absyn_Carray_mod_struct*_tmpC2=(struct Cyc_Absyn_Carray_mod_struct*)
_tmpC1;if(_tmpC2->tag != 0)goto _LL81;}_LL80:{const char*_tmp680;void*_tmp67F;(
_tmp67F=0,Cyc_fprintf(Cyc_stderr,((_tmp680="Carray_mod ",_tag_dyneither(_tmp680,
sizeof(char),12))),_tag_dyneither(_tmp67F,sizeof(void*),0)));}goto _LL7E;_LL81: {
struct Cyc_Absyn_ConstArray_mod_struct*_tmpC3=(struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpC1;if(_tmpC3->tag != 1)goto _LL83;}_LL82:{const char*_tmp683;void*_tmp682;(
_tmp682=0,Cyc_fprintf(Cyc_stderr,((_tmp683="ConstArray_mod ",_tag_dyneither(
_tmp683,sizeof(char),16))),_tag_dyneither(_tmp682,sizeof(void*),0)));}goto _LL7E;
_LL83: {struct Cyc_Absyn_Function_mod_struct*_tmpC4=(struct Cyc_Absyn_Function_mod_struct*)
_tmpC1;if(_tmpC4->tag != 3)goto _LL85;else{_tmpC5=(void*)_tmpC4->f1;{struct Cyc_Absyn_WithTypes_struct*
_tmpC6=(struct Cyc_Absyn_WithTypes_struct*)_tmpC5;if(_tmpC6->tag != 1)goto _LL85;
else{_tmpC7=_tmpC6->f1;}};}}_LL84:{const char*_tmp686;void*_tmp685;(_tmp685=0,Cyc_fprintf(
Cyc_stderr,((_tmp686="Function_mod(",_tag_dyneither(_tmp686,sizeof(char),14))),
_tag_dyneither(_tmp685,sizeof(void*),0)));}for(0;_tmpC7 != 0;_tmpC7=_tmpC7->tl){
struct Cyc_Core_Opt*_tmpD2=(*((struct _tuple7*)_tmpC7->hd)).f1;if(_tmpD2 == 0){
const char*_tmp689;void*_tmp688;(_tmp688=0,Cyc_fprintf(Cyc_stderr,((_tmp689="?",
_tag_dyneither(_tmp689,sizeof(char),2))),_tag_dyneither(_tmp688,sizeof(void*),0)));}
else{void*_tmp68A;(_tmp68A=0,Cyc_fprintf(Cyc_stderr,*((struct _dyneither_ptr*)
_tmpD2->v),_tag_dyneither(_tmp68A,sizeof(void*),0)));}if(_tmpC7->tl != 0){const
char*_tmp68D;void*_tmp68C;(_tmp68C=0,Cyc_fprintf(Cyc_stderr,((_tmp68D=",",
_tag_dyneither(_tmp68D,sizeof(char),2))),_tag_dyneither(_tmp68C,sizeof(void*),0)));}}{
const char*_tmp690;void*_tmp68F;(_tmp68F=0,Cyc_fprintf(Cyc_stderr,((_tmp690=") ",
_tag_dyneither(_tmp690,sizeof(char),3))),_tag_dyneither(_tmp68F,sizeof(void*),0)));}
goto _LL7E;_LL85: {struct Cyc_Absyn_Function_mod_struct*_tmpC8=(struct Cyc_Absyn_Function_mod_struct*)
_tmpC1;if(_tmpC8->tag != 3)goto _LL87;}_LL86:{const char*_tmp693;void*_tmp692;(
_tmp692=0,Cyc_fprintf(Cyc_stderr,((_tmp693="Function_mod()",_tag_dyneither(
_tmp693,sizeof(char),15))),_tag_dyneither(_tmp692,sizeof(void*),0)));}goto _LL7E;
_LL87: {struct Cyc_Absyn_Attributes_mod_struct*_tmpC9=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpC1;if(_tmpC9->tag != 5)goto _LL89;}_LL88:{const char*_tmp696;void*_tmp695;(
_tmp695=0,Cyc_fprintf(Cyc_stderr,((_tmp696="Attributes_mod ",_tag_dyneither(
_tmp696,sizeof(char),16))),_tag_dyneither(_tmp695,sizeof(void*),0)));}goto _LL7E;
_LL89: {struct Cyc_Absyn_TypeParams_mod_struct*_tmpCA=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpC1;if(_tmpCA->tag != 4)goto _LL8B;}_LL8A:{const char*_tmp699;void*_tmp698;(
_tmp698=0,Cyc_fprintf(Cyc_stderr,((_tmp699="TypeParams_mod ",_tag_dyneither(
_tmp699,sizeof(char),16))),_tag_dyneither(_tmp698,sizeof(void*),0)));}goto _LL7E;
_LL8B: {struct Cyc_Absyn_Pointer_mod_struct*_tmpCB=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmpC1;if(_tmpCB->tag != 2)goto _LL7E;}_LL8C:{const char*_tmp69C;void*_tmp69B;(
_tmp69B=0,Cyc_fprintf(Cyc_stderr,((_tmp69C="Pointer_mod ",_tag_dyneither(_tmp69C,
sizeof(char),13))),_tag_dyneither(_tmp69B,sizeof(void*),0)));}goto _LL7E;_LL7E:;}{
const char*_tmp69F;void*_tmp69E;(_tmp69E=0,Cyc_fprintf(Cyc_stderr,((_tmp69F="\n",
_tag_dyneither(_tmp69F,sizeof(char),2))),_tag_dyneither(_tmp69E,sizeof(void*),0)));};}
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct
Cyc_List_List*tms);struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct
Cyc_PP_Doc*d,struct Cyc_List_List*tms){if(tms == 0)return d;{struct Cyc_PP_Doc*rest=
Cyc_Absynpp_dtms2doc(0,d,tms->tl);const char*_tmp6A4;const char*_tmp6A3;struct Cyc_PP_Doc*
_tmp6A2[3];struct Cyc_PP_Doc*p_rest=(_tmp6A2[2]=Cyc_PP_text(((_tmp6A3=")",
_tag_dyneither(_tmp6A3,sizeof(char),2)))),((_tmp6A2[1]=rest,((_tmp6A2[0]=Cyc_PP_text(((
_tmp6A4="(",_tag_dyneither(_tmp6A4,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp6A2,sizeof(struct Cyc_PP_Doc*),3)))))));void*_tmpE4=(void*)tms->hd;union Cyc_Absyn_Constraint*
_tmpE6;struct Cyc_Absyn_Exp*_tmpE8;union Cyc_Absyn_Constraint*_tmpE9;void*_tmpEB;
struct Cyc_List_List*_tmpED;struct Cyc_List_List*_tmpEF;struct Cyc_Position_Segment*
_tmpF0;int _tmpF1;struct Cyc_Absyn_PtrAtts _tmpF3;void*_tmpF4;union Cyc_Absyn_Constraint*
_tmpF5;union Cyc_Absyn_Constraint*_tmpF6;union Cyc_Absyn_Constraint*_tmpF7;struct
Cyc_Absyn_Tqual _tmpF8;_LL8E: {struct Cyc_Absyn_Carray_mod_struct*_tmpE5=(struct
Cyc_Absyn_Carray_mod_struct*)_tmpE4;if(_tmpE5->tag != 0)goto _LL90;else{_tmpE6=
_tmpE5->f1;}}_LL8F: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{const
char*_tmp6A9;const char*_tmp6A8;struct Cyc_PP_Doc*_tmp6A7[2];return(_tmp6A7[1]=((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpE6)?Cyc_PP_text(((
_tmp6A8="[]@zeroterm",_tag_dyneither(_tmp6A8,sizeof(char),12)))): Cyc_PP_text(((
_tmp6A9="[]",_tag_dyneither(_tmp6A9,sizeof(char),3)))),((_tmp6A7[0]=rest,Cyc_PP_cat(
_tag_dyneither(_tmp6A7,sizeof(struct Cyc_PP_Doc*),2)))));};_LL90: {struct Cyc_Absyn_ConstArray_mod_struct*
_tmpE7=(struct Cyc_Absyn_ConstArray_mod_struct*)_tmpE4;if(_tmpE7->tag != 1)goto
_LL92;else{_tmpE8=_tmpE7->f1;_tmpE9=_tmpE7->f2;}}_LL91: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{const char*_tmp6B0;const char*_tmp6AF;const char*_tmp6AE;
struct Cyc_PP_Doc*_tmp6AD[4];return(_tmp6AD[3]=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmpE9)?Cyc_PP_text(((_tmp6AE="]@zeroterm",
_tag_dyneither(_tmp6AE,sizeof(char),11)))): Cyc_PP_text(((_tmp6AF="]",
_tag_dyneither(_tmp6AF,sizeof(char),2)))),((_tmp6AD[2]=Cyc_Absynpp_exp2doc(
_tmpE8),((_tmp6AD[1]=Cyc_PP_text(((_tmp6B0="[",_tag_dyneither(_tmp6B0,sizeof(
char),2)))),((_tmp6AD[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6AD,sizeof(struct Cyc_PP_Doc*),
4)))))))));};_LL92: {struct Cyc_Absyn_Function_mod_struct*_tmpEA=(struct Cyc_Absyn_Function_mod_struct*)
_tmpE4;if(_tmpEA->tag != 3)goto _LL94;else{_tmpEB=(void*)_tmpEA->f1;}}_LL93: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{void*_tmp100=_tmpEB;struct Cyc_List_List*_tmp102;int _tmp103;
struct Cyc_Absyn_VarargInfo*_tmp104;struct Cyc_Core_Opt*_tmp105;struct Cyc_List_List*
_tmp106;struct Cyc_List_List*_tmp108;struct Cyc_Position_Segment*_tmp109;_LL9B: {
struct Cyc_Absyn_WithTypes_struct*_tmp101=(struct Cyc_Absyn_WithTypes_struct*)
_tmp100;if(_tmp101->tag != 1)goto _LL9D;else{_tmp102=_tmp101->f1;_tmp103=_tmp101->f2;
_tmp104=_tmp101->f3;_tmp105=_tmp101->f4;_tmp106=_tmp101->f5;}}_LL9C: {struct Cyc_PP_Doc*
_tmp6B1[2];return(_tmp6B1[1]=Cyc_Absynpp_funargs2doc(_tmp102,_tmp103,_tmp104,
_tmp105,_tmp106),((_tmp6B1[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6B1,sizeof(
struct Cyc_PP_Doc*),2)))));}_LL9D: {struct Cyc_Absyn_NoTypes_struct*_tmp107=(
struct Cyc_Absyn_NoTypes_struct*)_tmp100;if(_tmp107->tag != 0)goto _LL9A;else{
_tmp108=_tmp107->f1;_tmp109=_tmp107->f2;}}_LL9E: {const char*_tmp6B8;const char*
_tmp6B7;const char*_tmp6B6;struct Cyc_PP_Doc*_tmp6B5[2];return(_tmp6B5[1]=Cyc_PP_group(((
_tmp6B8="(",_tag_dyneither(_tmp6B8,sizeof(char),2))),((_tmp6B7=")",
_tag_dyneither(_tmp6B7,sizeof(char),2))),((_tmp6B6=",",_tag_dyneither(_tmp6B6,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp108)),((
_tmp6B5[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6B5,sizeof(struct Cyc_PP_Doc*),2)))));}
_LL9A:;};_LL94: {struct Cyc_Absyn_Attributes_mod_struct*_tmpEC=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpE4;if(_tmpEC->tag != 5)goto _LL96;else{_tmpED=_tmpEC->f2;}}_LL95: switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LL9F: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
struct Cyc_PP_Doc*_tmp6B9[2];return(_tmp6B9[1]=Cyc_Absynpp_atts2doc(_tmpED),((
_tmp6B9[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6B9,sizeof(struct Cyc_PP_Doc*),2)))));};
case Cyc_Cyclone_Vc_c: _LLA0: if(Cyc_Absynpp_next_is_pointer(tms->tl)){struct Cyc_PP_Doc*
_tmp6BA[2];return(_tmp6BA[1]=rest,((_tmp6BA[0]=Cyc_Absynpp_callconv2doc(_tmpED),
Cyc_PP_cat(_tag_dyneither(_tmp6BA,sizeof(struct Cyc_PP_Doc*),2)))));}return rest;}
_LL96: {struct Cyc_Absyn_TypeParams_mod_struct*_tmpEE=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpE4;if(_tmpEE->tag != 4)goto _LL98;else{_tmpEF=_tmpEE->f1;_tmpF0=_tmpEE->f2;
_tmpF1=_tmpEE->f3;}}_LL97: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmpF1){struct Cyc_PP_Doc*_tmp6BB[2];return(_tmp6BB[1]=Cyc_Absynpp_ktvars2doc(
_tmpEF),((_tmp6BB[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6BB,sizeof(struct Cyc_PP_Doc*),
2)))));}else{struct Cyc_PP_Doc*_tmp6BC[2];return(_tmp6BC[1]=Cyc_Absynpp_tvars2doc(
_tmpEF),((_tmp6BC[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6BC,sizeof(struct Cyc_PP_Doc*),
2)))));}_LL98: {struct Cyc_Absyn_Pointer_mod_struct*_tmpF2=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmpE4;if(_tmpF2->tag != 2)goto _LL8D;else{_tmpF3=_tmpF2->f1;_tmpF4=_tmpF3.rgn;
_tmpF5=_tmpF3.nullable;_tmpF6=_tmpF3.bounds;_tmpF7=_tmpF3.zero_term;_tmpF8=
_tmpF2->f2;}}_LL99: {struct Cyc_PP_Doc*ptr;{void*_tmp113=((void*(*)(void*y,union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmpF6);
struct Cyc_Absyn_Exp*_tmp116;_LLA3: {struct Cyc_Absyn_DynEither_b_struct*_tmp114=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp113;if(_tmp114->tag != 0)goto _LLA5;}_LLA4:
ptr=Cyc_Absynpp_question();goto _LLA2;_LLA5: {struct Cyc_Absyn_Upper_b_struct*
_tmp115=(struct Cyc_Absyn_Upper_b_struct*)_tmp113;if(_tmp115->tag != 1)goto _LLA2;
else{_tmp116=_tmp115->f1;}}_LLA6:{const char*_tmp6BE;const char*_tmp6BD;ptr=Cyc_PP_text(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmpF5)?(
_tmp6BE="*",_tag_dyneither(_tmp6BE,sizeof(char),2)):((_tmp6BD="@",_tag_dyneither(
_tmp6BD,sizeof(char),2))));}{unsigned int _tmp11A;int _tmp11B;struct _tuple10
_tmp119=Cyc_Evexp_eval_const_uint_exp(_tmp116);_tmp11A=_tmp119.f1;_tmp11B=
_tmp119.f2;if(!_tmp11B  || _tmp11A != 1){struct Cyc_PP_Doc*_tmp6BF[4];ptr=((_tmp6BF[
3]=Cyc_Absynpp_rb(),((_tmp6BF[2]=Cyc_Absynpp_exp2doc(_tmp116),((_tmp6BF[1]=Cyc_Absynpp_lb(),((
_tmp6BF[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6BF,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LLA2;};_LLA2:;}if(Cyc_Absynpp_print_zeroterm){if(!is_char_ptr  && ((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF7)){const char*
_tmp6C2;struct Cyc_PP_Doc*_tmp6C1[2];ptr=((_tmp6C1[1]=Cyc_PP_text(((_tmp6C2="@zeroterm ",
_tag_dyneither(_tmp6C2,sizeof(char),11)))),((_tmp6C1[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp6C1,sizeof(struct Cyc_PP_Doc*),2))))));}else{if(is_char_ptr
 && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF7)){
const char*_tmp6C5;struct Cyc_PP_Doc*_tmp6C4[2];ptr=((_tmp6C4[1]=Cyc_PP_text(((
_tmp6C5="@nozeroterm ",_tag_dyneither(_tmp6C5,sizeof(char),13)))),((_tmp6C4[0]=
ptr,Cyc_PP_cat(_tag_dyneither(_tmp6C4,sizeof(struct Cyc_PP_Doc*),2))))));}}}{void*
_tmp121=Cyc_Tcutil_compress(_tmpF4);_LLA8: {struct Cyc_Absyn_HeapRgn_struct*
_tmp122=(struct Cyc_Absyn_HeapRgn_struct*)_tmp121;if(_tmp122->tag != 21)goto _LLAA;}
_LLA9: goto _LLA7;_LLAA:;_LLAB: {const char*_tmp6C8;struct Cyc_PP_Doc*_tmp6C7[3];ptr=((
_tmp6C7[2]=Cyc_PP_text(((_tmp6C8=" ",_tag_dyneither(_tmp6C8,sizeof(char),2)))),((
_tmp6C7[1]=Cyc_Absynpp_typ2doc(_tmpF4),((_tmp6C7[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp6C7,sizeof(struct Cyc_PP_Doc*),3))))))));}_LLA7:;}{struct Cyc_PP_Doc*
_tmp6C9[2];ptr=((_tmp6C9[1]=Cyc_Absynpp_tqual2doc(_tmpF8),((_tmp6C9[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp6C9,sizeof(struct Cyc_PP_Doc*),2))))));}{struct Cyc_PP_Doc*
_tmp6CA[2];return(_tmp6CA[1]=rest,((_tmp6CA[0]=ptr,Cyc_PP_cat(_tag_dyneither(
_tmp6CA,sizeof(struct Cyc_PP_Doc*),2)))));};}_LL8D:;};}struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(
void*t);struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){void*_tmp12A=Cyc_Tcutil_compress(
t);_LLAD: {struct Cyc_Absyn_HeapRgn_struct*_tmp12B=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp12A;if(_tmp12B->tag != 21)goto _LLAF;}_LLAE: {const char*_tmp6CB;return Cyc_PP_text(((
_tmp6CB="`H",_tag_dyneither(_tmp6CB,sizeof(char),3))));}_LLAF: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp12C=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp12A;if(_tmp12C->tag != 22)goto
_LLB1;}_LLB0: {const char*_tmp6CC;return Cyc_PP_text(((_tmp6CC="`U",_tag_dyneither(
_tmp6CC,sizeof(char),3))));}_LLB1:;_LLB2: return Cyc_Absynpp_ntyp2doc(t);_LLAC:;}
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**
effects,void*t);static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,
struct Cyc_List_List**effects,void*t){void*_tmp12F=Cyc_Tcutil_compress(t);void*
_tmp131;struct Cyc_List_List*_tmp133;_LLB4: {struct Cyc_Absyn_AccessEff_struct*
_tmp130=(struct Cyc_Absyn_AccessEff_struct*)_tmp12F;if(_tmp130->tag != 23)goto
_LLB6;else{_tmp131=(void*)_tmp130->f1;}}_LLB5:{struct Cyc_List_List*_tmp6CD;*
rgions=((_tmp6CD=_cycalloc(sizeof(*_tmp6CD)),((_tmp6CD->hd=Cyc_Absynpp_rgn2doc(
_tmp131),((_tmp6CD->tl=*rgions,_tmp6CD))))));}goto _LLB3;_LLB6: {struct Cyc_Absyn_JoinEff_struct*
_tmp132=(struct Cyc_Absyn_JoinEff_struct*)_tmp12F;if(_tmp132->tag != 24)goto _LLB8;
else{_tmp133=_tmp132->f1;}}_LLB7: for(0;_tmp133 != 0;_tmp133=_tmp133->tl){Cyc_Absynpp_effects2docs(
rgions,effects,(void*)_tmp133->hd);}goto _LLB3;_LLB8:;_LLB9:{struct Cyc_List_List*
_tmp6CE;*effects=((_tmp6CE=_cycalloc(sizeof(*_tmp6CE)),((_tmp6CE->hd=Cyc_Absynpp_typ2doc(
t),((_tmp6CE->tl=*effects,_tmp6CE))))));}goto _LLB3;_LLB3:;}struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(
void*t);struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){struct Cyc_List_List*rgions=
0;struct Cyc_List_List*effects=0;Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
effects);if(rgions == 0  && effects != 0){const char*_tmp6D1;const char*_tmp6D0;const
char*_tmp6CF;return Cyc_PP_group(((_tmp6CF="",_tag_dyneither(_tmp6CF,sizeof(char),
1))),((_tmp6D0="",_tag_dyneither(_tmp6D0,sizeof(char),1))),((_tmp6D1="+",
_tag_dyneither(_tmp6D1,sizeof(char),2))),effects);}else{const char*_tmp6D2;struct
Cyc_PP_Doc*_tmp139=Cyc_Absynpp_group_braces(((_tmp6D2=",",_tag_dyneither(_tmp6D2,
sizeof(char),2))),rgions);struct Cyc_List_List*_tmp6D6;const char*_tmp6D5;const
char*_tmp6D4;const char*_tmp6D3;return Cyc_PP_group(((_tmp6D3="",_tag_dyneither(
_tmp6D3,sizeof(char),1))),((_tmp6D4="",_tag_dyneither(_tmp6D4,sizeof(char),1))),((
_tmp6D5="+",_tag_dyneither(_tmp6D5,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,((
_tmp6D6=_cycalloc(sizeof(*_tmp6D6)),((_tmp6D6->hd=_tmp139,((_tmp6D6->tl=0,
_tmp6D6))))))));}}struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind
k);struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){switch(k){
case Cyc_Absyn_StructA: _LLBA: {const char*_tmp6D7;return Cyc_PP_text(((_tmp6D7="struct ",
_tag_dyneither(_tmp6D7,sizeof(char),8))));}case Cyc_Absyn_UnionA: _LLBB: {const
char*_tmp6D8;return Cyc_PP_text(((_tmp6D8="union ",_tag_dyneither(_tmp6D8,sizeof(
char),7))));}}}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);struct Cyc_PP_Doc*
Cyc_Absynpp_ntyp2doc(void*t){struct Cyc_PP_Doc*s;{void*_tmp141=t;struct Cyc_Core_Opt*
_tmp147;struct Cyc_Core_Opt*_tmp148;int _tmp149;struct Cyc_Core_Opt*_tmp14A;struct
Cyc_Absyn_Tvar*_tmp14C;struct Cyc_Absyn_DatatypeInfo _tmp14E;union Cyc_Absyn_DatatypeInfoU
_tmp14F;struct Cyc_List_List*_tmp150;struct Cyc_Absyn_DatatypeFieldInfo _tmp152;
union Cyc_Absyn_DatatypeFieldInfoU _tmp153;struct Cyc_List_List*_tmp154;enum Cyc_Absyn_Sign
_tmp156;enum Cyc_Absyn_Size_of _tmp157;int _tmp15A;struct Cyc_List_List*_tmp15C;
struct Cyc_Absyn_AggrInfo _tmp15E;union Cyc_Absyn_AggrInfoU _tmp15F;struct Cyc_List_List*
_tmp160;enum Cyc_Absyn_AggrKind _tmp162;struct Cyc_List_List*_tmp163;struct Cyc_List_List*
_tmp165;struct _tuple0*_tmp167;struct Cyc_Absyn_Exp*_tmp169;struct _tuple0*_tmp16B;
struct Cyc_List_List*_tmp16C;struct Cyc_Absyn_Typedefdecl*_tmp16D;void*_tmp16F;
void*_tmp171;void*_tmp172;void*_tmp174;void*_tmp178;_LLBE: {struct Cyc_Absyn_ArrayType_struct*
_tmp142=(struct Cyc_Absyn_ArrayType_struct*)_tmp141;if(_tmp142->tag != 9)goto _LLC0;}
_LLBF: {const char*_tmp6D9;return Cyc_PP_text(((_tmp6D9="[[[array]]]",
_tag_dyneither(_tmp6D9,sizeof(char),12))));}_LLC0: {struct Cyc_Absyn_FnType_struct*
_tmp143=(struct Cyc_Absyn_FnType_struct*)_tmp141;if(_tmp143->tag != 10)goto _LLC2;}
_LLC1: return Cyc_PP_nil_doc();_LLC2: {struct Cyc_Absyn_PointerType_struct*_tmp144=(
struct Cyc_Absyn_PointerType_struct*)_tmp141;if(_tmp144->tag != 5)goto _LLC4;}_LLC3:
return Cyc_PP_nil_doc();_LLC4: {struct Cyc_Absyn_VoidType_struct*_tmp145=(struct
Cyc_Absyn_VoidType_struct*)_tmp141;if(_tmp145->tag != 0)goto _LLC6;}_LLC5:{const
char*_tmp6DA;s=Cyc_PP_text(((_tmp6DA="void",_tag_dyneither(_tmp6DA,sizeof(char),
5))));}goto _LLBD;_LLC6: {struct Cyc_Absyn_Evar_struct*_tmp146=(struct Cyc_Absyn_Evar_struct*)
_tmp141;if(_tmp146->tag != 1)goto _LLC8;else{_tmp147=_tmp146->f1;_tmp148=_tmp146->f2;
_tmp149=_tmp146->f3;_tmp14A=_tmp146->f4;}}_LLC7: if(_tmp148 != 0)return Cyc_Absynpp_ntyp2doc((
void*)_tmp148->v);else{const char*_tmp6EA;const char*_tmp6E9;const char*_tmp6E8;
struct Cyc_Int_pa_struct _tmp6E7;void*_tmp6E6[1];const char*_tmp6E5;const char*
_tmp6E4;struct Cyc_PP_Doc*_tmp6E3[6];s=((_tmp6E3[5]=_tmp147 == 0?Cyc_Absynpp_question():
Cyc_Absynpp_kind2doc((enum Cyc_Absyn_Kind)_tmp147->v),((_tmp6E3[4]=Cyc_PP_text(((
_tmp6E4=")::",_tag_dyneither(_tmp6E4,sizeof(char),4)))),((_tmp6E3[3]=(!Cyc_Absynpp_print_full_evars
 || _tmp14A == 0)?Cyc_PP_nil_doc(): Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)
_tmp14A->v),((_tmp6E3[2]=Cyc_PP_text((struct _dyneither_ptr)((_tmp6E7.tag=1,((
_tmp6E7.f1=(unsigned long)_tmp149,((_tmp6E6[0]=& _tmp6E7,Cyc_aprintf(((_tmp6E5="%d",
_tag_dyneither(_tmp6E5,sizeof(char),3))),_tag_dyneither(_tmp6E6,sizeof(void*),1))))))))),((
_tmp6E3[1]=Cyc_PP_text(((_tmp6E8="(",_tag_dyneither(_tmp6E8,sizeof(char),2)))),((
_tmp6E3[0]=Cyc_Absynpp_print_for_cycdoc?Cyc_PP_text(((_tmp6E9="\\%",
_tag_dyneither(_tmp6E9,sizeof(char),3)))): Cyc_PP_text(((_tmp6EA="%",
_tag_dyneither(_tmp6EA,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6E3,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));}goto _LLBD;_LLC8: {struct Cyc_Absyn_VarType_struct*
_tmp14B=(struct Cyc_Absyn_VarType_struct*)_tmp141;if(_tmp14B->tag != 2)goto _LLCA;
else{_tmp14C=_tmp14B->f1;}}_LLC9: s=Cyc_PP_textptr(_tmp14C->name);if(Cyc_Absynpp_print_all_kinds){
const char*_tmp6ED;struct Cyc_PP_Doc*_tmp6EC[3];s=((_tmp6EC[2]=Cyc_Absynpp_kindbound2doc(
_tmp14C->kind),((_tmp6EC[1]=Cyc_PP_text(((_tmp6ED="::",_tag_dyneither(_tmp6ED,
sizeof(char),3)))),((_tmp6EC[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp6EC,sizeof(
struct Cyc_PP_Doc*),3))))))));}if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(
_tmp14C)){const char*_tmp6F2;const char*_tmp6F1;struct Cyc_PP_Doc*_tmp6F0[3];s=((
_tmp6F0[2]=Cyc_PP_text(((_tmp6F1=" */",_tag_dyneither(_tmp6F1,sizeof(char),4)))),((
_tmp6F0[1]=s,((_tmp6F0[0]=Cyc_PP_text(((_tmp6F2="_ /* ",_tag_dyneither(_tmp6F2,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp6F0,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLBD;_LLCA: {struct Cyc_Absyn_DatatypeType_struct*_tmp14D=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp141;if(_tmp14D->tag != 3)goto _LLCC;else{_tmp14E=_tmp14D->f1;_tmp14F=_tmp14E.datatype_info;
_tmp150=_tmp14E.targs;}}_LLCB:{union Cyc_Absyn_DatatypeInfoU _tmp18A=_tmp14F;
struct Cyc_Absyn_UnknownDatatypeInfo _tmp18B;struct _tuple0*_tmp18C;int _tmp18D;
struct Cyc_Absyn_Datatypedecl**_tmp18E;struct Cyc_Absyn_Datatypedecl*_tmp18F;
struct Cyc_Absyn_Datatypedecl _tmp190;struct _tuple0*_tmp191;int _tmp192;_LLF3: if((
_tmp18A.UnknownDatatype).tag != 1)goto _LLF5;_tmp18B=(struct Cyc_Absyn_UnknownDatatypeInfo)(
_tmp18A.UnknownDatatype).val;_tmp18C=_tmp18B.name;_tmp18D=_tmp18B.is_extensible;
_LLF4: _tmp191=_tmp18C;_tmp192=_tmp18D;goto _LLF6;_LLF5: if((_tmp18A.KnownDatatype).tag
!= 2)goto _LLF2;_tmp18E=(struct Cyc_Absyn_Datatypedecl**)(_tmp18A.KnownDatatype).val;
_tmp18F=*_tmp18E;_tmp190=*_tmp18F;_tmp191=_tmp190.name;_tmp192=_tmp190.is_extensible;
_LLF6: {const char*_tmp6F3;struct Cyc_PP_Doc*_tmp193=Cyc_PP_text(((_tmp6F3="datatype ",
_tag_dyneither(_tmp6F3,sizeof(char),10))));const char*_tmp6F4;struct Cyc_PP_Doc*
_tmp194=_tmp192?Cyc_PP_text(((_tmp6F4="@extensible ",_tag_dyneither(_tmp6F4,
sizeof(char),13)))): Cyc_PP_nil_doc();{struct Cyc_PP_Doc*_tmp6F5[4];s=((_tmp6F5[3]=
Cyc_Absynpp_tps2doc(_tmp150),((_tmp6F5[2]=Cyc_Absynpp_qvar2doc(_tmp191),((
_tmp6F5[1]=_tmp193,((_tmp6F5[0]=_tmp194,Cyc_PP_cat(_tag_dyneither(_tmp6F5,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLF2;}_LLF2:;}goto _LLBD;_LLCC: {
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp151=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp141;if(_tmp151->tag != 4)goto _LLCE;else{_tmp152=_tmp151->f1;_tmp153=_tmp152.field_info;
_tmp154=_tmp152.targs;}}_LLCD:{union Cyc_Absyn_DatatypeFieldInfoU _tmp198=_tmp153;
struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp199;struct _tuple0*_tmp19A;struct
_tuple0*_tmp19B;int _tmp19C;struct _tuple1 _tmp19D;struct Cyc_Absyn_Datatypedecl*
_tmp19E;struct Cyc_Absyn_Datatypedecl _tmp19F;struct _tuple0*_tmp1A0;int _tmp1A1;
struct Cyc_Absyn_Datatypefield*_tmp1A2;struct Cyc_Absyn_Datatypefield _tmp1A3;
struct _tuple0*_tmp1A4;_LLF8: if((_tmp198.UnknownDatatypefield).tag != 1)goto _LLFA;
_tmp199=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp198.UnknownDatatypefield).val;
_tmp19A=_tmp199.datatype_name;_tmp19B=_tmp199.field_name;_tmp19C=_tmp199.is_extensible;
_LLF9: _tmp1A0=_tmp19A;_tmp1A1=_tmp19C;_tmp1A4=_tmp19B;goto _LLFB;_LLFA: if((
_tmp198.KnownDatatypefield).tag != 2)goto _LLF7;_tmp19D=(struct _tuple1)(_tmp198.KnownDatatypefield).val;
_tmp19E=_tmp19D.f1;_tmp19F=*_tmp19E;_tmp1A0=_tmp19F.name;_tmp1A1=_tmp19F.is_extensible;
_tmp1A2=_tmp19D.f2;_tmp1A3=*_tmp1A2;_tmp1A4=_tmp1A3.name;_LLFB: {const char*
_tmp6F7;const char*_tmp6F6;struct Cyc_PP_Doc*_tmp1A5=Cyc_PP_text(_tmp1A1?(_tmp6F7="@extensible datatype ",
_tag_dyneither(_tmp6F7,sizeof(char),22)):((_tmp6F6="datatype ",_tag_dyneither(
_tmp6F6,sizeof(char),10))));{const char*_tmp6FA;struct Cyc_PP_Doc*_tmp6F9[4];s=((
_tmp6F9[3]=Cyc_Absynpp_qvar2doc(_tmp1A4),((_tmp6F9[2]=Cyc_PP_text(((_tmp6FA=".",
_tag_dyneither(_tmp6FA,sizeof(char),2)))),((_tmp6F9[1]=Cyc_Absynpp_qvar2doc(
_tmp1A0),((_tmp6F9[0]=_tmp1A5,Cyc_PP_cat(_tag_dyneither(_tmp6F9,sizeof(struct Cyc_PP_Doc*),
4))))))))));}goto _LLF7;}_LLF7:;}goto _LLBD;_LLCE: {struct Cyc_Absyn_IntType_struct*
_tmp155=(struct Cyc_Absyn_IntType_struct*)_tmp141;if(_tmp155->tag != 6)goto _LLD0;
else{_tmp156=_tmp155->f1;_tmp157=_tmp155->f2;}}_LLCF: {struct _dyneither_ptr sns;
struct _dyneither_ptr ts;switch(_tmp156){case Cyc_Absyn_None: _LLFC: goto _LLFD;case
Cyc_Absyn_Signed: _LLFD:{const char*_tmp6FB;sns=((_tmp6FB="",_tag_dyneither(
_tmp6FB,sizeof(char),1)));}break;case Cyc_Absyn_Unsigned: _LLFE:{const char*_tmp6FC;
sns=((_tmp6FC="unsigned ",_tag_dyneither(_tmp6FC,sizeof(char),10)));}break;}
switch(_tmp157){case Cyc_Absyn_Char_sz: _LL100: switch(_tmp156){case Cyc_Absyn_None:
_LL102:{const char*_tmp6FD;sns=((_tmp6FD="",_tag_dyneither(_tmp6FD,sizeof(char),1)));}
break;case Cyc_Absyn_Signed: _LL103:{const char*_tmp6FE;sns=((_tmp6FE="signed ",
_tag_dyneither(_tmp6FE,sizeof(char),8)));}break;case Cyc_Absyn_Unsigned: _LL104:{
const char*_tmp6FF;sns=((_tmp6FF="unsigned ",_tag_dyneither(_tmp6FF,sizeof(char),
10)));}break;}{const char*_tmp700;ts=((_tmp700="char",_tag_dyneither(_tmp700,
sizeof(char),5)));}break;case Cyc_Absyn_Short_sz: _LL101:{const char*_tmp701;ts=((
_tmp701="short",_tag_dyneither(_tmp701,sizeof(char),6)));}break;case Cyc_Absyn_Int_sz:
_LL106:{const char*_tmp702;ts=((_tmp702="int",_tag_dyneither(_tmp702,sizeof(char),
4)));}break;case Cyc_Absyn_Long_sz: _LL107:{const char*_tmp703;ts=((_tmp703="long",
_tag_dyneither(_tmp703,sizeof(char),5)));}break;case Cyc_Absyn_LongLong_sz: _LL108:
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL10A:{const char*_tmp704;
ts=((_tmp704="long long",_tag_dyneither(_tmp704,sizeof(char),10)));}break;case
Cyc_Cyclone_Vc_c: _LL10B:{const char*_tmp705;ts=((_tmp705="__int64",_tag_dyneither(
_tmp705,sizeof(char),8)));}break;}break;}{const char*_tmp70A;void*_tmp709[2];
struct Cyc_String_pa_struct _tmp708;struct Cyc_String_pa_struct _tmp707;s=Cyc_PP_text((
struct _dyneither_ptr)((_tmp707.tag=0,((_tmp707.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)ts),((_tmp708.tag=0,((_tmp708.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)sns),((_tmp709[0]=& _tmp708,((_tmp709[1]=& _tmp707,Cyc_aprintf(((
_tmp70A="%s%s",_tag_dyneither(_tmp70A,sizeof(char),5))),_tag_dyneither(_tmp709,
sizeof(void*),2)))))))))))))));}goto _LLBD;}_LLD0: {struct Cyc_Absyn_FloatType_struct*
_tmp158=(struct Cyc_Absyn_FloatType_struct*)_tmp141;if(_tmp158->tag != 7)goto _LLD2;}
_LLD1:{const char*_tmp70B;s=Cyc_PP_text(((_tmp70B="float",_tag_dyneither(_tmp70B,
sizeof(char),6))));}goto _LLBD;_LLD2: {struct Cyc_Absyn_DoubleType_struct*_tmp159=(
struct Cyc_Absyn_DoubleType_struct*)_tmp141;if(_tmp159->tag != 8)goto _LLD4;else{
_tmp15A=_tmp159->f1;}}_LLD3: if(_tmp15A){const char*_tmp70C;s=Cyc_PP_text(((
_tmp70C="long double",_tag_dyneither(_tmp70C,sizeof(char),12))));}else{const char*
_tmp70D;s=Cyc_PP_text(((_tmp70D="double",_tag_dyneither(_tmp70D,sizeof(char),7))));}
goto _LLBD;_LLD4: {struct Cyc_Absyn_TupleType_struct*_tmp15B=(struct Cyc_Absyn_TupleType_struct*)
_tmp141;if(_tmp15B->tag != 11)goto _LLD6;else{_tmp15C=_tmp15B->f1;}}_LLD5:{struct
Cyc_PP_Doc*_tmp70E[2];s=((_tmp70E[1]=Cyc_Absynpp_args2doc(_tmp15C),((_tmp70E[0]=
Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp70E,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LLBD;_LLD6: {struct Cyc_Absyn_AggrType_struct*_tmp15D=(struct Cyc_Absyn_AggrType_struct*)
_tmp141;if(_tmp15D->tag != 12)goto _LLD8;else{_tmp15E=_tmp15D->f1;_tmp15F=_tmp15E.aggr_info;
_tmp160=_tmp15E.targs;}}_LLD7: {enum Cyc_Absyn_AggrKind _tmp1BE;struct _tuple0*
_tmp1BF;struct _tuple9 _tmp1BD=Cyc_Absyn_aggr_kinded_name(_tmp15F);_tmp1BE=_tmp1BD.f1;
_tmp1BF=_tmp1BD.f2;{struct Cyc_PP_Doc*_tmp70F[3];s=((_tmp70F[2]=Cyc_Absynpp_tps2doc(
_tmp160),((_tmp70F[1]=Cyc_Absynpp_qvar2doc(_tmp1BF),((_tmp70F[0]=Cyc_Absynpp_aggr_kind2doc(
_tmp1BE),Cyc_PP_cat(_tag_dyneither(_tmp70F,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLBD;}_LLD8: {struct Cyc_Absyn_AnonAggrType_struct*_tmp161=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp141;if(_tmp161->tag != 13)goto _LLDA;else{_tmp162=_tmp161->f1;_tmp163=_tmp161->f2;}}
_LLD9:{struct Cyc_PP_Doc*_tmp710[4];s=((_tmp710[3]=Cyc_Absynpp_rb(),((_tmp710[2]=
Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp163)),((_tmp710[1]=Cyc_Absynpp_lb(),((
_tmp710[0]=Cyc_Absynpp_aggr_kind2doc(_tmp162),Cyc_PP_cat(_tag_dyneither(_tmp710,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLBD;_LLDA: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp164=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp141;if(_tmp164->tag != 15)goto
_LLDC;else{_tmp165=_tmp164->f1;}}_LLDB:{const char*_tmp713;struct Cyc_PP_Doc*
_tmp712[4];s=((_tmp712[3]=Cyc_Absynpp_rb(),((_tmp712[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(
_tmp165)),((_tmp712[1]=Cyc_Absynpp_lb(),((_tmp712[0]=Cyc_PP_text(((_tmp713="enum ",
_tag_dyneither(_tmp713,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp712,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLBD;_LLDC: {struct Cyc_Absyn_EnumType_struct*
_tmp166=(struct Cyc_Absyn_EnumType_struct*)_tmp141;if(_tmp166->tag != 14)goto _LLDE;
else{_tmp167=_tmp166->f1;}}_LLDD:{const char*_tmp716;struct Cyc_PP_Doc*_tmp715[2];
s=((_tmp715[1]=Cyc_Absynpp_qvar2doc(_tmp167),((_tmp715[0]=Cyc_PP_text(((_tmp716="enum ",
_tag_dyneither(_tmp716,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp715,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLBD;_LLDE: {struct Cyc_Absyn_ValueofType_struct*
_tmp168=(struct Cyc_Absyn_ValueofType_struct*)_tmp141;if(_tmp168->tag != 19)goto
_LLE0;else{_tmp169=_tmp168->f1;}}_LLDF:{const char*_tmp71B;const char*_tmp71A;
struct Cyc_PP_Doc*_tmp719[3];s=((_tmp719[2]=Cyc_PP_text(((_tmp71A=")",
_tag_dyneither(_tmp71A,sizeof(char),2)))),((_tmp719[1]=Cyc_Absynpp_exp2doc(
_tmp169),((_tmp719[0]=Cyc_PP_text(((_tmp71B="valueof_t(",_tag_dyneither(_tmp71B,
sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp719,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LLBD;_LLE0: {struct Cyc_Absyn_TypedefType_struct*_tmp16A=(struct
Cyc_Absyn_TypedefType_struct*)_tmp141;if(_tmp16A->tag != 18)goto _LLE2;else{
_tmp16B=_tmp16A->f1;_tmp16C=_tmp16A->f2;_tmp16D=_tmp16A->f3;}}_LLE1:{struct Cyc_PP_Doc*
_tmp71C[2];s=((_tmp71C[1]=Cyc_Absynpp_tps2doc(_tmp16C),((_tmp71C[0]=Cyc_Absynpp_qvar2doc(
_tmp16B),Cyc_PP_cat(_tag_dyneither(_tmp71C,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLBD;_LLE2: {struct Cyc_Absyn_RgnHandleType_struct*_tmp16E=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp141;if(_tmp16E->tag != 16)goto _LLE4;else{_tmp16F=(void*)_tmp16E->f1;}}_LLE3:{
const char*_tmp721;const char*_tmp720;struct Cyc_PP_Doc*_tmp71F[3];s=((_tmp71F[2]=
Cyc_PP_text(((_tmp720=">",_tag_dyneither(_tmp720,sizeof(char),2)))),((_tmp71F[1]=
Cyc_Absynpp_rgn2doc(_tmp16F),((_tmp71F[0]=Cyc_PP_text(((_tmp721="region_t<",
_tag_dyneither(_tmp721,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp71F,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLBD;_LLE4: {struct Cyc_Absyn_DynRgnType_struct*
_tmp170=(struct Cyc_Absyn_DynRgnType_struct*)_tmp141;if(_tmp170->tag != 17)goto
_LLE6;else{_tmp171=(void*)_tmp170->f1;_tmp172=(void*)_tmp170->f2;}}_LLE5:{const
char*_tmp728;const char*_tmp727;const char*_tmp726;struct Cyc_PP_Doc*_tmp725[5];s=((
_tmp725[4]=Cyc_PP_text(((_tmp726=">",_tag_dyneither(_tmp726,sizeof(char),2)))),((
_tmp725[3]=Cyc_Absynpp_rgn2doc(_tmp172),((_tmp725[2]=Cyc_PP_text(((_tmp727=",",
_tag_dyneither(_tmp727,sizeof(char),2)))),((_tmp725[1]=Cyc_Absynpp_rgn2doc(
_tmp171),((_tmp725[0]=Cyc_PP_text(((_tmp728="dynregion_t<",_tag_dyneither(
_tmp728,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp725,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LLBD;_LLE6: {struct Cyc_Absyn_TagType_struct*_tmp173=(struct
Cyc_Absyn_TagType_struct*)_tmp141;if(_tmp173->tag != 20)goto _LLE8;else{_tmp174=(
void*)_tmp173->f1;}}_LLE7:{const char*_tmp72D;const char*_tmp72C;struct Cyc_PP_Doc*
_tmp72B[3];s=((_tmp72B[2]=Cyc_PP_text(((_tmp72C=">",_tag_dyneither(_tmp72C,
sizeof(char),2)))),((_tmp72B[1]=Cyc_Absynpp_typ2doc(_tmp174),((_tmp72B[0]=Cyc_PP_text(((
_tmp72D="tag_t<",_tag_dyneither(_tmp72D,sizeof(char),7)))),Cyc_PP_cat(
_tag_dyneither(_tmp72B,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLBD;_LLE8: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp175=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp141;if(_tmp175->tag != 22)goto _LLEA;}_LLE9: goto _LLEB;_LLEA: {struct Cyc_Absyn_HeapRgn_struct*
_tmp176=(struct Cyc_Absyn_HeapRgn_struct*)_tmp141;if(_tmp176->tag != 21)goto _LLEC;}
_LLEB: s=Cyc_Absynpp_rgn2doc(t);goto _LLBD;_LLEC: {struct Cyc_Absyn_RgnsEff_struct*
_tmp177=(struct Cyc_Absyn_RgnsEff_struct*)_tmp141;if(_tmp177->tag != 25)goto _LLEE;
else{_tmp178=(void*)_tmp177->f1;}}_LLED:{const char*_tmp732;const char*_tmp731;
struct Cyc_PP_Doc*_tmp730[3];s=((_tmp730[2]=Cyc_PP_text(((_tmp731=")",
_tag_dyneither(_tmp731,sizeof(char),2)))),((_tmp730[1]=Cyc_Absynpp_typ2doc(
_tmp178),((_tmp730[0]=Cyc_PP_text(((_tmp732="regions(",_tag_dyneither(_tmp732,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp730,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLBD;_LLEE: {struct Cyc_Absyn_AccessEff_struct*_tmp179=(struct Cyc_Absyn_AccessEff_struct*)
_tmp141;if(_tmp179->tag != 23)goto _LLF0;}_LLEF: goto _LLF1;_LLF0: {struct Cyc_Absyn_JoinEff_struct*
_tmp17A=(struct Cyc_Absyn_JoinEff_struct*)_tmp141;if(_tmp17A->tag != 24)goto _LLBD;}
_LLF1: s=Cyc_Absynpp_eff2doc(t);goto _LLBD;_LLBD:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(
struct Cyc_Core_Opt*vo);struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct Cyc_Core_Opt*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*((struct _dyneither_ptr*)vo->v));}
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple11*cmp);struct Cyc_PP_Doc*
Cyc_Absynpp_rgn_cmp2doc(struct _tuple11*cmp){struct _tuple11 _tmp1D8;void*_tmp1D9;
void*_tmp1DA;struct _tuple11*_tmp1D7=cmp;_tmp1D8=*_tmp1D7;_tmp1D9=_tmp1D8.f1;
_tmp1DA=_tmp1D8.f2;{const char*_tmp735;struct Cyc_PP_Doc*_tmp734[3];return(_tmp734[
2]=Cyc_Absynpp_rgn2doc(_tmp1DA),((_tmp734[1]=Cyc_PP_text(((_tmp735=" > ",
_tag_dyneither(_tmp735,sizeof(char),4)))),((_tmp734[0]=Cyc_Absynpp_rgn2doc(
_tmp1D9),Cyc_PP_cat(_tag_dyneither(_tmp734,sizeof(struct Cyc_PP_Doc*),3)))))));};}
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po);struct Cyc_PP_Doc*
Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){const char*_tmp738;const char*
_tmp737;const char*_tmp736;return Cyc_PP_group(((_tmp736="",_tag_dyneither(_tmp736,
sizeof(char),1))),((_tmp737="",_tag_dyneither(_tmp737,sizeof(char),1))),((
_tmp738=",",_tag_dyneither(_tmp738,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,
po));}struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple7*t);struct Cyc_PP_Doc*
Cyc_Absynpp_funarg2doc(struct _tuple7*t){struct Cyc_Core_Opt*_tmp739;struct Cyc_Core_Opt*
dopt=(*t).f1 == 0?0:((_tmp739=_cycalloc(sizeof(*_tmp739)),((_tmp739->v=Cyc_PP_text(*((
struct _dyneither_ptr*)((struct Cyc_Core_Opt*)_check_null((*t).f1))->v)),_tmp739))));
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po);struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po){struct Cyc_List_List*
_tmp1E1=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple7*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);struct Cyc_PP_Doc*
eff_doc;if(c_varargs){const char*_tmp73C;struct Cyc_List_List*_tmp73B;_tmp1E1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp1E1,((_tmp73B=_cycalloc(sizeof(*_tmp73B)),((_tmp73B->hd=Cyc_PP_text(((
_tmp73C="...",_tag_dyneither(_tmp73C,sizeof(char),4)))),((_tmp73B->tl=0,_tmp73B)))))));}
else{if(cyc_varargs != 0){const char*_tmp745;const char*_tmp744;const char*_tmp743;
struct _tuple7*_tmp742;struct Cyc_PP_Doc*_tmp741[3];struct Cyc_PP_Doc*_tmp1E4=(
_tmp741[2]=Cyc_Absynpp_funarg2doc(((_tmp742=_cycalloc(sizeof(*_tmp742)),((
_tmp742->f1=cyc_varargs->name,((_tmp742->f2=cyc_varargs->tq,((_tmp742->f3=
cyc_varargs->type,_tmp742))))))))),((_tmp741[1]=cyc_varargs->inject?Cyc_PP_text(((
_tmp743=" inject ",_tag_dyneither(_tmp743,sizeof(char),9)))): Cyc_PP_text(((
_tmp744=" ",_tag_dyneither(_tmp744,sizeof(char),2)))),((_tmp741[0]=Cyc_PP_text(((
_tmp745="...",_tag_dyneither(_tmp745,sizeof(char),4)))),Cyc_PP_cat(
_tag_dyneither(_tmp741,sizeof(struct Cyc_PP_Doc*),3)))))));struct Cyc_List_List*
_tmp746;_tmp1E1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp1E1,((_tmp746=_cycalloc(sizeof(*_tmp746)),((_tmp746->hd=
_tmp1E4,((_tmp746->tl=0,_tmp746)))))));}}{const char*_tmp749;const char*_tmp748;
const char*_tmp747;struct Cyc_PP_Doc*_tmp1EB=Cyc_PP_group(((_tmp747="",
_tag_dyneither(_tmp747,sizeof(char),1))),((_tmp748="",_tag_dyneither(_tmp748,
sizeof(char),1))),((_tmp749=",",_tag_dyneither(_tmp749,sizeof(char),2))),_tmp1E1);
if(effopt != 0  && Cyc_Absynpp_print_all_effects){const char*_tmp74C;struct Cyc_PP_Doc*
_tmp74B[3];_tmp1EB=((_tmp74B[2]=Cyc_Absynpp_eff2doc((void*)effopt->v),((_tmp74B[
1]=Cyc_PP_text(((_tmp74C=";",_tag_dyneither(_tmp74C,sizeof(char),2)))),((_tmp74B[
0]=_tmp1EB,Cyc_PP_cat(_tag_dyneither(_tmp74B,sizeof(struct Cyc_PP_Doc*),3))))))));}
if(rgn_po != 0){const char*_tmp74F;struct Cyc_PP_Doc*_tmp74E[3];_tmp1EB=((_tmp74E[2]=
Cyc_Absynpp_rgnpo2doc(rgn_po),((_tmp74E[1]=Cyc_PP_text(((_tmp74F=":",
_tag_dyneither(_tmp74F,sizeof(char),2)))),((_tmp74E[0]=_tmp1EB,Cyc_PP_cat(
_tag_dyneither(_tmp74E,sizeof(struct Cyc_PP_Doc*),3))))))));}{const char*_tmp754;
const char*_tmp753;struct Cyc_PP_Doc*_tmp752[3];return(_tmp752[2]=Cyc_PP_text(((
_tmp753=")",_tag_dyneither(_tmp753,sizeof(char),2)))),((_tmp752[1]=_tmp1EB,((
_tmp752[0]=Cyc_PP_text(((_tmp754="(",_tag_dyneither(_tmp754,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp752,sizeof(struct Cyc_PP_Doc*),3)))))));};};}struct
_tuple7*Cyc_Absynpp_arg_mk_opt(struct _tuple12*arg);struct _tuple7*Cyc_Absynpp_arg_mk_opt(
struct _tuple12*arg){struct Cyc_Core_Opt*_tmp757;struct _tuple7*_tmp756;return(
_tmp756=_cycalloc(sizeof(*_tmp756)),((_tmp756->f1=((_tmp757=_cycalloc(sizeof(*
_tmp757)),((_tmp757->v=(*arg).f1,_tmp757)))),((_tmp756->f2=(*arg).f2,((_tmp756->f3=(*
arg).f3,_tmp756)))))));}struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct
_dyneither_ptr*v);struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dyneither_ptr*v){
return Cyc_PP_text(*v);}struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*
q);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){struct Cyc_List_List*
_tmp1F8=0;int match;{union Cyc_Absyn_Nmspace _tmp1F9=(*q).f1;int _tmp1FA;struct Cyc_List_List*
_tmp1FB;struct Cyc_List_List*_tmp1FC;_LL10E: if((_tmp1F9.Loc_n).tag != 3)goto _LL110;
_tmp1FA=(int)(_tmp1F9.Loc_n).val;_LL10F: _tmp1FB=0;goto _LL111;_LL110: if((_tmp1F9.Rel_n).tag
!= 1)goto _LL112;_tmp1FB=(struct Cyc_List_List*)(_tmp1F9.Rel_n).val;_LL111: match=0;
_tmp1F8=_tmp1FB;goto _LL10D;_LL112: if((_tmp1F9.Abs_n).tag != 2)goto _LL10D;_tmp1FC=(
struct Cyc_List_List*)(_tmp1F9.Abs_n).val;_LL113: match=Cyc_Absynpp_use_curr_namespace
 && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1FC,Cyc_Absynpp_curr_namespace);{
struct Cyc_List_List*_tmp758;_tmp1F8=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?(
_tmp758=_cycalloc(sizeof(*_tmp758)),((_tmp758->hd=Cyc_Absynpp_cyc_stringptr,((
_tmp758->tl=_tmp1FC,_tmp758))))): _tmp1FC;}goto _LL10D;_LL10D:;}if(Cyc_Absynpp_qvar_to_Cids){
const char*_tmp75A;struct Cyc_List_List*_tmp759;return(struct _dyneither_ptr)Cyc_str_sepstr(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp1F8,((_tmp759=_cycalloc(sizeof(*_tmp759)),((_tmp759->hd=(*q).f2,((_tmp759->tl=
0,_tmp759))))))),((_tmp75A="_",_tag_dyneither(_tmp75A,sizeof(char),2))));}else{
if(match)return*(*q).f2;else{const char*_tmp75C;struct Cyc_List_List*_tmp75B;
return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1F8,((_tmp75B=_cycalloc(sizeof(*
_tmp75B)),((_tmp75B->hd=(*q).f2,((_tmp75B->tl=0,_tmp75B))))))),((_tmp75C="::",
_tag_dyneither(_tmp75C,sizeof(char),3))));}}}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(
struct _tuple0*q);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){return
Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(
struct _tuple0*q);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _dyneither_ptr _tmp202=Cyc_Absynpp_qvar2string(q);if(Cyc_PP_tex_output){
const char*_tmp75E;const char*_tmp75D;return Cyc_PP_text_width((struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp75D="\\textbf{",
_tag_dyneither(_tmp75D,sizeof(char),9))),(struct _dyneither_ptr)_tmp202),((
_tmp75E="}",_tag_dyneither(_tmp75E,sizeof(char),2)))),(int)Cyc_strlen((struct
_dyneither_ptr)_tmp202));}else{return Cyc_PP_text(_tmp202);}}struct _dyneither_ptr
Cyc_Absynpp_typedef_name2string(struct _tuple0*v);struct _dyneither_ptr Cyc_Absynpp_typedef_name2string(
struct _tuple0*v){if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
if(Cyc_Absynpp_use_curr_namespace){union Cyc_Absyn_Nmspace _tmp205=(*v).f1;int
_tmp206;struct Cyc_List_List*_tmp207;struct Cyc_List_List*_tmp208;_LL115: if((
_tmp205.Loc_n).tag != 3)goto _LL117;_tmp206=(int)(_tmp205.Loc_n).val;_LL116: goto
_LL118;_LL117: if((_tmp205.Rel_n).tag != 1)goto _LL119;_tmp207=(struct Cyc_List_List*)(
_tmp205.Rel_n).val;if(_tmp207 != 0)goto _LL119;_LL118: return*(*v).f2;_LL119: if((
_tmp205.Abs_n).tag != 2)goto _LL11B;_tmp208=(struct Cyc_List_List*)(_tmp205.Abs_n).val;
_LL11A: if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct
Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,
_tmp208,Cyc_Absynpp_curr_namespace)== 0)return*(*v).f2;else{goto _LL11C;}_LL11B:;
_LL11C: {const char*_tmp75F;return(struct _dyneither_ptr)Cyc_strconcat(((_tmp75F="/* bad namespace : */ ",
_tag_dyneither(_tmp75F,sizeof(char),23))),(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
v));}_LL114:;}else{return*(*v).f2;}}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(
struct _tuple0*v);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(
struct _tuple0*v);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*
v){struct _dyneither_ptr _tmp20A=Cyc_Absynpp_typedef_name2string(v);if(Cyc_PP_tex_output){
const char*_tmp761;const char*_tmp760;return Cyc_PP_text_width((struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp760="\\textbf{",
_tag_dyneither(_tmp760,sizeof(char),9))),(struct _dyneither_ptr)_tmp20A),((
_tmp761="}",_tag_dyneither(_tmp761,sizeof(char),2)))),(int)Cyc_strlen((struct
_dyneither_ptr)_tmp20A));}else{return Cyc_PP_text(_tmp20A);}}struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(
void*t);struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){return Cyc_Absynpp_tqtd2doc(
Cyc_Absyn_empty_tqual(0),t,0);}int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e);
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){void*_tmp20D=e->r;enum Cyc_Absyn_Primop
_tmp212;struct Cyc_Absyn_Exp*_tmp21D;struct Cyc_Absyn_Exp*_tmp21F;_LL11E: {struct
Cyc_Absyn_Const_e_struct*_tmp20E=(struct Cyc_Absyn_Const_e_struct*)_tmp20D;if(
_tmp20E->tag != 0)goto _LL120;}_LL11F: goto _LL121;_LL120: {struct Cyc_Absyn_Var_e_struct*
_tmp20F=(struct Cyc_Absyn_Var_e_struct*)_tmp20D;if(_tmp20F->tag != 1)goto _LL122;}
_LL121: goto _LL123;_LL122: {struct Cyc_Absyn_UnknownId_e_struct*_tmp210=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmp20D;if(_tmp210->tag != 2)goto _LL124;}_LL123: return 10000;_LL124: {struct Cyc_Absyn_Primop_e_struct*
_tmp211=(struct Cyc_Absyn_Primop_e_struct*)_tmp20D;if(_tmp211->tag != 3)goto _LL126;
else{_tmp212=_tmp211->f1;}}_LL125: switch(_tmp212){case Cyc_Absyn_Plus: _LL170:
return 100;case Cyc_Absyn_Times: _LL171: return 110;case Cyc_Absyn_Minus: _LL172: return
100;case Cyc_Absyn_Div: _LL173: goto _LL174;case Cyc_Absyn_Mod: _LL174: return 110;case
Cyc_Absyn_Eq: _LL175: goto _LL176;case Cyc_Absyn_Neq: _LL176: return 70;case Cyc_Absyn_Gt:
_LL177: goto _LL178;case Cyc_Absyn_Lt: _LL178: goto _LL179;case Cyc_Absyn_Gte: _LL179:
goto _LL17A;case Cyc_Absyn_Lte: _LL17A: return 80;case Cyc_Absyn_Not: _LL17B: goto _LL17C;
case Cyc_Absyn_Bitnot: _LL17C: return 130;case Cyc_Absyn_Bitand: _LL17D: return 60;case
Cyc_Absyn_Bitor: _LL17E: return 40;case Cyc_Absyn_Bitxor: _LL17F: return 50;case Cyc_Absyn_Bitlshift:
_LL180: return 90;case Cyc_Absyn_Bitlrshift: _LL181: return 80;case Cyc_Absyn_Bitarshift:
_LL182: return 80;case Cyc_Absyn_Numelts: _LL183: return 140;}_LL126: {struct Cyc_Absyn_AssignOp_e_struct*
_tmp213=(struct Cyc_Absyn_AssignOp_e_struct*)_tmp20D;if(_tmp213->tag != 4)goto
_LL128;}_LL127: return 20;_LL128: {struct Cyc_Absyn_Increment_e_struct*_tmp214=(
struct Cyc_Absyn_Increment_e_struct*)_tmp20D;if(_tmp214->tag != 5)goto _LL12A;}
_LL129: return 130;_LL12A: {struct Cyc_Absyn_Conditional_e_struct*_tmp215=(struct
Cyc_Absyn_Conditional_e_struct*)_tmp20D;if(_tmp215->tag != 6)goto _LL12C;}_LL12B:
return 30;_LL12C: {struct Cyc_Absyn_And_e_struct*_tmp216=(struct Cyc_Absyn_And_e_struct*)
_tmp20D;if(_tmp216->tag != 7)goto _LL12E;}_LL12D: return 35;_LL12E: {struct Cyc_Absyn_Or_e_struct*
_tmp217=(struct Cyc_Absyn_Or_e_struct*)_tmp20D;if(_tmp217->tag != 8)goto _LL130;}
_LL12F: return 30;_LL130: {struct Cyc_Absyn_SeqExp_e_struct*_tmp218=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp20D;if(_tmp218->tag != 9)goto _LL132;}_LL131: return 10;_LL132: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp219=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp20D;if(_tmp219->tag != 10)goto
_LL134;}_LL133: goto _LL135;_LL134: {struct Cyc_Absyn_FnCall_e_struct*_tmp21A=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp20D;if(_tmp21A->tag != 11)goto _LL136;}_LL135:
return 140;_LL136: {struct Cyc_Absyn_Throw_e_struct*_tmp21B=(struct Cyc_Absyn_Throw_e_struct*)
_tmp20D;if(_tmp21B->tag != 12)goto _LL138;}_LL137: return 130;_LL138: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp21C=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp20D;if(_tmp21C->tag != 13)
goto _LL13A;else{_tmp21D=_tmp21C->f1;}}_LL139: return Cyc_Absynpp_exp_prec(_tmp21D);
_LL13A: {struct Cyc_Absyn_Instantiate_e_struct*_tmp21E=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp20D;if(_tmp21E->tag != 14)goto _LL13C;else{_tmp21F=_tmp21E->f1;}}_LL13B: return
Cyc_Absynpp_exp_prec(_tmp21F);_LL13C: {struct Cyc_Absyn_Cast_e_struct*_tmp220=(
struct Cyc_Absyn_Cast_e_struct*)_tmp20D;if(_tmp220->tag != 15)goto _LL13E;}_LL13D:
return 120;_LL13E: {struct Cyc_Absyn_New_e_struct*_tmp221=(struct Cyc_Absyn_New_e_struct*)
_tmp20D;if(_tmp221->tag != 17)goto _LL140;}_LL13F: return 15;_LL140: {struct Cyc_Absyn_Address_e_struct*
_tmp222=(struct Cyc_Absyn_Address_e_struct*)_tmp20D;if(_tmp222->tag != 16)goto
_LL142;}_LL141: goto _LL143;_LL142: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp223=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp20D;if(_tmp223->tag != 18)goto _LL144;}
_LL143: goto _LL145;_LL144: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp224=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp20D;if(_tmp224->tag != 19)goto _LL146;}_LL145: goto _LL147;_LL146: {struct Cyc_Absyn_Valueof_e_struct*
_tmp225=(struct Cyc_Absyn_Valueof_e_struct*)_tmp20D;if(_tmp225->tag != 40)goto
_LL148;}_LL147: goto _LL149;_LL148: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp226=(
struct Cyc_Absyn_Tagcheck_e_struct*)_tmp20D;if(_tmp226->tag != 39)goto _LL14A;}
_LL149: goto _LL14B;_LL14A: {struct Cyc_Absyn_Offsetof_e_struct*_tmp227=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp20D;if(_tmp227->tag != 20)goto _LL14C;}_LL14B: goto _LL14D;_LL14C: {struct Cyc_Absyn_Gentyp_e_struct*
_tmp228=(struct Cyc_Absyn_Gentyp_e_struct*)_tmp20D;if(_tmp228->tag != 21)goto
_LL14E;}_LL14D: goto _LL14F;_LL14E: {struct Cyc_Absyn_Deref_e_struct*_tmp229=(
struct Cyc_Absyn_Deref_e_struct*)_tmp20D;if(_tmp229->tag != 22)goto _LL150;}_LL14F:
return 130;_LL150: {struct Cyc_Absyn_AggrMember_e_struct*_tmp22A=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp20D;if(_tmp22A->tag != 23)goto _LL152;}_LL151: goto _LL153;_LL152: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp22B=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp20D;if(_tmp22B->tag != 24)goto
_LL154;}_LL153: goto _LL155;_LL154: {struct Cyc_Absyn_Subscript_e_struct*_tmp22C=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp20D;if(_tmp22C->tag != 25)goto _LL156;}
_LL155: return 140;_LL156: {struct Cyc_Absyn_Tuple_e_struct*_tmp22D=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp20D;if(_tmp22D->tag != 26)goto _LL158;}_LL157: return 150;_LL158: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp22E=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp20D;if(_tmp22E->tag != 27)goto
_LL15A;}_LL159: goto _LL15B;_LL15A: {struct Cyc_Absyn_Array_e_struct*_tmp22F=(
struct Cyc_Absyn_Array_e_struct*)_tmp20D;if(_tmp22F->tag != 28)goto _LL15C;}_LL15B:
goto _LL15D;_LL15C: {struct Cyc_Absyn_Comprehension_e_struct*_tmp230=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp20D;if(_tmp230->tag != 29)goto _LL15E;}_LL15D: goto _LL15F;_LL15E: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp231=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp20D;if(_tmp231->tag != 30)goto
_LL160;}_LL15F: goto _LL161;_LL160: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp232=(
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp20D;if(_tmp232->tag != 31)goto _LL162;}
_LL161: goto _LL163;_LL162: {struct Cyc_Absyn_Datatype_e_struct*_tmp233=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp20D;if(_tmp233->tag != 32)goto _LL164;}_LL163: goto _LL165;_LL164: {struct Cyc_Absyn_Enum_e_struct*
_tmp234=(struct Cyc_Absyn_Enum_e_struct*)_tmp20D;if(_tmp234->tag != 33)goto _LL166;}
_LL165: goto _LL167;_LL166: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp235=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp20D;if(_tmp235->tag != 34)goto _LL168;}_LL167: goto _LL169;_LL168: {struct Cyc_Absyn_Malloc_e_struct*
_tmp236=(struct Cyc_Absyn_Malloc_e_struct*)_tmp20D;if(_tmp236->tag != 35)goto
_LL16A;}_LL169: goto _LL16B;_LL16A: {struct Cyc_Absyn_Swap_e_struct*_tmp237=(struct
Cyc_Absyn_Swap_e_struct*)_tmp20D;if(_tmp237->tag != 36)goto _LL16C;}_LL16B: goto
_LL16D;_LL16C: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp238=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp20D;if(_tmp238->tag != 37)goto _LL16E;}_LL16D: return 140;_LL16E: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp239=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp20D;if(_tmp239->tag != 38)goto
_LL11D;}_LL16F: return 10000;_LL11D:;}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct
Cyc_Absyn_Exp*e);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int
inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*
s;{void*_tmp23A=e->r;union Cyc_Absyn_Cnst _tmp23C;struct _tuple0*_tmp23E;struct
_tuple0*_tmp240;enum Cyc_Absyn_Primop _tmp242;struct Cyc_List_List*_tmp243;struct
Cyc_Absyn_Exp*_tmp245;struct Cyc_Core_Opt*_tmp246;struct Cyc_Absyn_Exp*_tmp247;
struct Cyc_Absyn_Exp*_tmp249;enum Cyc_Absyn_Incrementor _tmp24A;struct Cyc_Absyn_Exp*
_tmp24C;struct Cyc_Absyn_Exp*_tmp24D;struct Cyc_Absyn_Exp*_tmp24E;struct Cyc_Absyn_Exp*
_tmp250;struct Cyc_Absyn_Exp*_tmp251;struct Cyc_Absyn_Exp*_tmp253;struct Cyc_Absyn_Exp*
_tmp254;struct Cyc_Absyn_Exp*_tmp256;struct Cyc_Absyn_Exp*_tmp257;struct Cyc_Absyn_Exp*
_tmp259;struct Cyc_List_List*_tmp25A;struct Cyc_Absyn_Exp*_tmp25C;struct Cyc_List_List*
_tmp25D;struct Cyc_Absyn_Exp*_tmp25F;struct Cyc_Absyn_Exp*_tmp261;struct Cyc_Absyn_Exp*
_tmp263;void*_tmp265;struct Cyc_Absyn_Exp*_tmp266;struct Cyc_Absyn_Exp*_tmp268;
struct Cyc_Absyn_Exp*_tmp26A;struct Cyc_Absyn_Exp*_tmp26B;void*_tmp26D;struct Cyc_Absyn_Exp*
_tmp26F;void*_tmp271;struct Cyc_Absyn_Exp*_tmp273;struct _dyneither_ptr*_tmp274;
void*_tmp276;void*_tmp277;struct _dyneither_ptr*_tmp279;void*_tmp27B;void*_tmp27C;
unsigned int _tmp27E;struct Cyc_List_List*_tmp280;void*_tmp281;struct Cyc_Absyn_Exp*
_tmp283;struct Cyc_Absyn_Exp*_tmp285;struct _dyneither_ptr*_tmp286;struct Cyc_Absyn_Exp*
_tmp288;struct _dyneither_ptr*_tmp289;struct Cyc_Absyn_Exp*_tmp28B;struct Cyc_Absyn_Exp*
_tmp28C;struct Cyc_List_List*_tmp28E;struct _tuple7*_tmp290;struct Cyc_List_List*
_tmp291;struct Cyc_List_List*_tmp293;struct Cyc_Absyn_Vardecl*_tmp295;struct Cyc_Absyn_Exp*
_tmp296;struct Cyc_Absyn_Exp*_tmp297;struct _tuple0*_tmp299;struct Cyc_List_List*
_tmp29A;struct Cyc_List_List*_tmp29B;struct Cyc_List_List*_tmp29D;struct Cyc_List_List*
_tmp29F;struct Cyc_Absyn_Datatypefield*_tmp2A0;struct _tuple0*_tmp2A2;struct
_tuple0*_tmp2A4;struct Cyc_Absyn_MallocInfo _tmp2A6;int _tmp2A7;struct Cyc_Absyn_Exp*
_tmp2A8;void**_tmp2A9;struct Cyc_Absyn_Exp*_tmp2AA;struct Cyc_Absyn_Exp*_tmp2AC;
struct Cyc_Absyn_Exp*_tmp2AD;struct Cyc_Core_Opt*_tmp2AF;struct Cyc_List_List*
_tmp2B0;struct Cyc_Absyn_Stmt*_tmp2B2;_LL186: {struct Cyc_Absyn_Const_e_struct*
_tmp23B=(struct Cyc_Absyn_Const_e_struct*)_tmp23A;if(_tmp23B->tag != 0)goto _LL188;
else{_tmp23C=_tmp23B->f1;}}_LL187: s=Cyc_Absynpp_cnst2doc(_tmp23C);goto _LL185;
_LL188: {struct Cyc_Absyn_Var_e_struct*_tmp23D=(struct Cyc_Absyn_Var_e_struct*)
_tmp23A;if(_tmp23D->tag != 1)goto _LL18A;else{_tmp23E=_tmp23D->f1;}}_LL189: _tmp240=
_tmp23E;goto _LL18B;_LL18A: {struct Cyc_Absyn_UnknownId_e_struct*_tmp23F=(struct
Cyc_Absyn_UnknownId_e_struct*)_tmp23A;if(_tmp23F->tag != 2)goto _LL18C;else{
_tmp240=_tmp23F->f1;}}_LL18B: s=Cyc_Absynpp_qvar2doc(_tmp240);goto _LL185;_LL18C: {
struct Cyc_Absyn_Primop_e_struct*_tmp241=(struct Cyc_Absyn_Primop_e_struct*)
_tmp23A;if(_tmp241->tag != 3)goto _LL18E;else{_tmp242=_tmp241->f1;_tmp243=_tmp241->f2;}}
_LL18D: s=Cyc_Absynpp_primapp2doc(myprec,_tmp242,_tmp243);goto _LL185;_LL18E: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp244=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp23A;if(_tmp244->tag != 4)goto _LL190;else{_tmp245=_tmp244->f1;_tmp246=_tmp244->f2;
_tmp247=_tmp244->f3;}}_LL18F:{const char*_tmp766;const char*_tmp765;struct Cyc_PP_Doc*
_tmp764[5];s=((_tmp764[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp247),((_tmp764[3]=
Cyc_PP_text(((_tmp765="= ",_tag_dyneither(_tmp765,sizeof(char),3)))),((_tmp764[2]=
_tmp246 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp246->v),((
_tmp764[1]=Cyc_PP_text(((_tmp766=" ",_tag_dyneither(_tmp766,sizeof(char),2)))),((
_tmp764[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp245),Cyc_PP_cat(_tag_dyneither(
_tmp764,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL185;_LL190: {struct Cyc_Absyn_Increment_e_struct*
_tmp248=(struct Cyc_Absyn_Increment_e_struct*)_tmp23A;if(_tmp248->tag != 5)goto
_LL192;else{_tmp249=_tmp248->f1;_tmp24A=_tmp248->f2;}}_LL191: {struct Cyc_PP_Doc*
_tmp2B6=Cyc_Absynpp_exp2doc_prec(myprec,_tmp249);switch(_tmp24A){case Cyc_Absyn_PreInc:
_LL1DA:{const char*_tmp769;struct Cyc_PP_Doc*_tmp768[2];s=((_tmp768[1]=_tmp2B6,((
_tmp768[0]=Cyc_PP_text(((_tmp769="++",_tag_dyneither(_tmp769,sizeof(char),3)))),
Cyc_PP_cat(_tag_dyneither(_tmp768,sizeof(struct Cyc_PP_Doc*),2))))));}break;case
Cyc_Absyn_PreDec: _LL1DB:{const char*_tmp76C;struct Cyc_PP_Doc*_tmp76B[2];s=((
_tmp76B[1]=_tmp2B6,((_tmp76B[0]=Cyc_PP_text(((_tmp76C="--",_tag_dyneither(
_tmp76C,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp76B,sizeof(struct Cyc_PP_Doc*),
2))))));}break;case Cyc_Absyn_PostInc: _LL1DC:{const char*_tmp76F;struct Cyc_PP_Doc*
_tmp76E[2];s=((_tmp76E[1]=Cyc_PP_text(((_tmp76F="++",_tag_dyneither(_tmp76F,
sizeof(char),3)))),((_tmp76E[0]=_tmp2B6,Cyc_PP_cat(_tag_dyneither(_tmp76E,
sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostDec: _LL1DD:{const char*
_tmp772;struct Cyc_PP_Doc*_tmp771[2];s=((_tmp771[1]=Cyc_PP_text(((_tmp772="--",
_tag_dyneither(_tmp772,sizeof(char),3)))),((_tmp771[0]=_tmp2B6,Cyc_PP_cat(
_tag_dyneither(_tmp771,sizeof(struct Cyc_PP_Doc*),2))))));}break;}goto _LL185;}
_LL192: {struct Cyc_Absyn_Conditional_e_struct*_tmp24B=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp23A;if(_tmp24B->tag != 6)goto _LL194;else{_tmp24C=_tmp24B->f1;_tmp24D=_tmp24B->f2;
_tmp24E=_tmp24B->f3;}}_LL193:{const char*_tmp777;const char*_tmp776;struct Cyc_PP_Doc*
_tmp775[5];s=((_tmp775[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp24E),((_tmp775[3]=
Cyc_PP_text(((_tmp776=" : ",_tag_dyneither(_tmp776,sizeof(char),4)))),((_tmp775[
2]=Cyc_Absynpp_exp2doc_prec(0,_tmp24D),((_tmp775[1]=Cyc_PP_text(((_tmp777=" ? ",
_tag_dyneither(_tmp777,sizeof(char),4)))),((_tmp775[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp24C),Cyc_PP_cat(_tag_dyneither(_tmp775,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL185;_LL194: {struct Cyc_Absyn_And_e_struct*_tmp24F=(struct Cyc_Absyn_And_e_struct*)
_tmp23A;if(_tmp24F->tag != 7)goto _LL196;else{_tmp250=_tmp24F->f1;_tmp251=_tmp24F->f2;}}
_LL195:{const char*_tmp77A;struct Cyc_PP_Doc*_tmp779[3];s=((_tmp779[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp251),((_tmp779[1]=Cyc_PP_text(((_tmp77A=" && ",_tag_dyneither(_tmp77A,
sizeof(char),5)))),((_tmp779[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp250),Cyc_PP_cat(
_tag_dyneither(_tmp779,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL185;_LL196: {
struct Cyc_Absyn_Or_e_struct*_tmp252=(struct Cyc_Absyn_Or_e_struct*)_tmp23A;if(
_tmp252->tag != 8)goto _LL198;else{_tmp253=_tmp252->f1;_tmp254=_tmp252->f2;}}
_LL197:{const char*_tmp77D;struct Cyc_PP_Doc*_tmp77C[3];s=((_tmp77C[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp254),((_tmp77C[1]=Cyc_PP_text(((_tmp77D=" || ",_tag_dyneither(_tmp77D,
sizeof(char),5)))),((_tmp77C[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp253),Cyc_PP_cat(
_tag_dyneither(_tmp77C,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL185;_LL198: {
struct Cyc_Absyn_SeqExp_e_struct*_tmp255=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp23A;if(_tmp255->tag != 9)goto _LL19A;else{_tmp256=_tmp255->f1;_tmp257=_tmp255->f2;}}
_LL199:{const char*_tmp784;const char*_tmp783;const char*_tmp782;struct Cyc_PP_Doc*
_tmp781[5];s=((_tmp781[4]=Cyc_PP_text(((_tmp782=")",_tag_dyneither(_tmp782,
sizeof(char),2)))),((_tmp781[3]=Cyc_Absynpp_exp2doc(_tmp257),((_tmp781[2]=Cyc_PP_text(((
_tmp783=", ",_tag_dyneither(_tmp783,sizeof(char),3)))),((_tmp781[1]=Cyc_Absynpp_exp2doc(
_tmp256),((_tmp781[0]=Cyc_PP_text(((_tmp784="(",_tag_dyneither(_tmp784,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp781,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL185;_LL19A: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp258=(struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp23A;if(_tmp258->tag != 10)goto _LL19C;else{_tmp259=_tmp258->f1;_tmp25A=_tmp258->f2;}}
_LL19B:{const char*_tmp789;const char*_tmp788;struct Cyc_PP_Doc*_tmp787[4];s=((
_tmp787[3]=Cyc_PP_text(((_tmp788=")",_tag_dyneither(_tmp788,sizeof(char),2)))),((
_tmp787[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp25A),((_tmp787[1]=Cyc_PP_text(((
_tmp789="(",_tag_dyneither(_tmp789,sizeof(char),2)))),((_tmp787[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp259),Cyc_PP_cat(_tag_dyneither(_tmp787,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL185;_LL19C: {struct Cyc_Absyn_FnCall_e_struct*_tmp25B=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp23A;if(_tmp25B->tag != 11)goto _LL19E;else{_tmp25C=_tmp25B->f1;_tmp25D=_tmp25B->f2;}}
_LL19D:{const char*_tmp78E;const char*_tmp78D;struct Cyc_PP_Doc*_tmp78C[4];s=((
_tmp78C[3]=Cyc_PP_text(((_tmp78D=")",_tag_dyneither(_tmp78D,sizeof(char),2)))),((
_tmp78C[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp25D),((_tmp78C[1]=Cyc_PP_text(((
_tmp78E="(",_tag_dyneither(_tmp78E,sizeof(char),2)))),((_tmp78C[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp25C),Cyc_PP_cat(_tag_dyneither(_tmp78C,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL185;_LL19E: {struct Cyc_Absyn_Throw_e_struct*_tmp25E=(struct Cyc_Absyn_Throw_e_struct*)
_tmp23A;if(_tmp25E->tag != 12)goto _LL1A0;else{_tmp25F=_tmp25E->f1;}}_LL19F:{const
char*_tmp791;struct Cyc_PP_Doc*_tmp790[2];s=((_tmp790[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp25F),((_tmp790[0]=Cyc_PP_text(((_tmp791="throw ",_tag_dyneither(
_tmp791,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp790,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LL185;_LL1A0: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp260=(
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp23A;if(_tmp260->tag != 13)goto _LL1A2;
else{_tmp261=_tmp260->f1;}}_LL1A1: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp261);
goto _LL185;_LL1A2: {struct Cyc_Absyn_Instantiate_e_struct*_tmp262=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp23A;if(_tmp262->tag != 14)goto _LL1A4;else{_tmp263=_tmp262->f1;}}_LL1A3: s=Cyc_Absynpp_exp2doc_prec(
inprec,_tmp263);goto _LL185;_LL1A4: {struct Cyc_Absyn_Cast_e_struct*_tmp264=(
struct Cyc_Absyn_Cast_e_struct*)_tmp23A;if(_tmp264->tag != 15)goto _LL1A6;else{
_tmp265=(void*)_tmp264->f1;_tmp266=_tmp264->f2;}}_LL1A5:{const char*_tmp796;const
char*_tmp795;struct Cyc_PP_Doc*_tmp794[4];s=((_tmp794[3]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp266),((_tmp794[2]=Cyc_PP_text(((_tmp795=")",_tag_dyneither(_tmp795,
sizeof(char),2)))),((_tmp794[1]=Cyc_Absynpp_typ2doc(_tmp265),((_tmp794[0]=Cyc_PP_text(((
_tmp796="(",_tag_dyneither(_tmp796,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp794,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL185;_LL1A6: {struct Cyc_Absyn_Address_e_struct*
_tmp267=(struct Cyc_Absyn_Address_e_struct*)_tmp23A;if(_tmp267->tag != 16)goto
_LL1A8;else{_tmp268=_tmp267->f1;}}_LL1A7:{const char*_tmp799;struct Cyc_PP_Doc*
_tmp798[2];s=((_tmp798[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp268),((_tmp798[0]=
Cyc_PP_text(((_tmp799="&",_tag_dyneither(_tmp799,sizeof(char),2)))),Cyc_PP_cat(
_tag_dyneither(_tmp798,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL185;_LL1A8: {
struct Cyc_Absyn_New_e_struct*_tmp269=(struct Cyc_Absyn_New_e_struct*)_tmp23A;if(
_tmp269->tag != 17)goto _LL1AA;else{_tmp26A=_tmp269->f1;_tmp26B=_tmp269->f2;}}
_LL1A9: if(_tmp26A == 0){const char*_tmp79C;struct Cyc_PP_Doc*_tmp79B[2];s=((_tmp79B[
1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp26B),((_tmp79B[0]=Cyc_PP_text(((_tmp79C="new ",
_tag_dyneither(_tmp79C,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp79B,
sizeof(struct Cyc_PP_Doc*),2))))));}else{const char*_tmp7A1;const char*_tmp7A0;
struct Cyc_PP_Doc*_tmp79F[4];s=((_tmp79F[3]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmp26B),((_tmp79F[2]=Cyc_PP_text(((_tmp7A0=") ",_tag_dyneither(_tmp7A0,sizeof(
char),3)))),((_tmp79F[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp26A),((
_tmp79F[0]=Cyc_PP_text(((_tmp7A1="rnew(",_tag_dyneither(_tmp7A1,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmp79F,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto
_LL185;_LL1AA: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp26C=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp23A;if(_tmp26C->tag != 18)goto _LL1AC;else{_tmp26D=(void*)_tmp26C->f1;}}_LL1AB:{
const char*_tmp7A6;const char*_tmp7A5;struct Cyc_PP_Doc*_tmp7A4[3];s=((_tmp7A4[2]=
Cyc_PP_text(((_tmp7A5=")",_tag_dyneither(_tmp7A5,sizeof(char),2)))),((_tmp7A4[1]=
Cyc_Absynpp_typ2doc(_tmp26D),((_tmp7A4[0]=Cyc_PP_text(((_tmp7A6="sizeof(",
_tag_dyneither(_tmp7A6,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7A4,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL185;_LL1AC: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp26E=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp23A;if(_tmp26E->tag != 19)goto
_LL1AE;else{_tmp26F=_tmp26E->f1;}}_LL1AD:{const char*_tmp7AB;const char*_tmp7AA;
struct Cyc_PP_Doc*_tmp7A9[3];s=((_tmp7A9[2]=Cyc_PP_text(((_tmp7AA=")",
_tag_dyneither(_tmp7AA,sizeof(char),2)))),((_tmp7A9[1]=Cyc_Absynpp_exp2doc(
_tmp26F),((_tmp7A9[0]=Cyc_PP_text(((_tmp7AB="sizeof(",_tag_dyneither(_tmp7AB,
sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7A9,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL185;_LL1AE: {struct Cyc_Absyn_Valueof_e_struct*_tmp270=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp23A;if(_tmp270->tag != 40)goto _LL1B0;else{_tmp271=(void*)_tmp270->f1;}}_LL1AF:{
const char*_tmp7B0;const char*_tmp7AF;struct Cyc_PP_Doc*_tmp7AE[3];s=((_tmp7AE[2]=
Cyc_PP_text(((_tmp7AF=")",_tag_dyneither(_tmp7AF,sizeof(char),2)))),((_tmp7AE[1]=
Cyc_Absynpp_typ2doc(_tmp271),((_tmp7AE[0]=Cyc_PP_text(((_tmp7B0="valueof(",
_tag_dyneither(_tmp7B0,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7AE,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL185;_LL1B0: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp272=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp23A;if(_tmp272->tag != 39)goto
_LL1B2;else{_tmp273=_tmp272->f1;_tmp274=_tmp272->f2;}}_LL1B1:{const char*_tmp7B7;
const char*_tmp7B6;const char*_tmp7B5;struct Cyc_PP_Doc*_tmp7B4[5];s=((_tmp7B4[4]=
Cyc_PP_text(((_tmp7B5=")",_tag_dyneither(_tmp7B5,sizeof(char),2)))),((_tmp7B4[3]=
Cyc_PP_textptr(_tmp274),((_tmp7B4[2]=Cyc_PP_text(((_tmp7B6=".",_tag_dyneither(
_tmp7B6,sizeof(char),2)))),((_tmp7B4[1]=Cyc_Absynpp_exp2doc(_tmp273),((_tmp7B4[0]=
Cyc_PP_text(((_tmp7B7="tagcheck(",_tag_dyneither(_tmp7B7,sizeof(char),10)))),Cyc_PP_cat(
_tag_dyneither(_tmp7B4,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL185;
_LL1B2: {struct Cyc_Absyn_Offsetof_e_struct*_tmp275=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp23A;if(_tmp275->tag != 20)goto _LL1B4;else{_tmp276=(void*)_tmp275->f1;_tmp277=(
void*)_tmp275->f2;{struct Cyc_Absyn_StructField_struct*_tmp278=(struct Cyc_Absyn_StructField_struct*)
_tmp277;if(_tmp278->tag != 0)goto _LL1B4;else{_tmp279=_tmp278->f1;}};}}_LL1B3:{
const char*_tmp7BE;const char*_tmp7BD;const char*_tmp7BC;struct Cyc_PP_Doc*_tmp7BB[5];
s=((_tmp7BB[4]=Cyc_PP_text(((_tmp7BC=")",_tag_dyneither(_tmp7BC,sizeof(char),2)))),((
_tmp7BB[3]=Cyc_PP_textptr(_tmp279),((_tmp7BB[2]=Cyc_PP_text(((_tmp7BD=",",
_tag_dyneither(_tmp7BD,sizeof(char),2)))),((_tmp7BB[1]=Cyc_Absynpp_typ2doc(
_tmp276),((_tmp7BB[0]=Cyc_PP_text(((_tmp7BE="offsetof(",_tag_dyneither(_tmp7BE,
sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7BB,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LL185;_LL1B4: {struct Cyc_Absyn_Offsetof_e_struct*_tmp27A=(
struct Cyc_Absyn_Offsetof_e_struct*)_tmp23A;if(_tmp27A->tag != 20)goto _LL1B6;else{
_tmp27B=(void*)_tmp27A->f1;_tmp27C=(void*)_tmp27A->f2;{struct Cyc_Absyn_TupleIndex_struct*
_tmp27D=(struct Cyc_Absyn_TupleIndex_struct*)_tmp27C;if(_tmp27D->tag != 1)goto
_LL1B6;else{_tmp27E=_tmp27D->f1;}};}}_LL1B5:{const char*_tmp7CC;const char*_tmp7CB;
struct Cyc_Int_pa_struct _tmp7CA;void*_tmp7C9[1];const char*_tmp7C8;const char*
_tmp7C7;struct Cyc_PP_Doc*_tmp7C6[5];s=((_tmp7C6[4]=Cyc_PP_text(((_tmp7C7=")",
_tag_dyneither(_tmp7C7,sizeof(char),2)))),((_tmp7C6[3]=Cyc_PP_text((struct
_dyneither_ptr)((_tmp7CA.tag=1,((_tmp7CA.f1=(unsigned long)((int)_tmp27E),((
_tmp7C9[0]=& _tmp7CA,Cyc_aprintf(((_tmp7C8="%d",_tag_dyneither(_tmp7C8,sizeof(
char),3))),_tag_dyneither(_tmp7C9,sizeof(void*),1))))))))),((_tmp7C6[2]=Cyc_PP_text(((
_tmp7CB=",",_tag_dyneither(_tmp7CB,sizeof(char),2)))),((_tmp7C6[1]=Cyc_Absynpp_typ2doc(
_tmp27B),((_tmp7C6[0]=Cyc_PP_text(((_tmp7CC="offsetof(",_tag_dyneither(_tmp7CC,
sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7C6,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LL185;_LL1B6: {struct Cyc_Absyn_Gentyp_e_struct*_tmp27F=(
struct Cyc_Absyn_Gentyp_e_struct*)_tmp23A;if(_tmp27F->tag != 21)goto _LL1B8;else{
_tmp280=_tmp27F->f1;_tmp281=(void*)_tmp27F->f2;}}_LL1B7:{const char*_tmp7D1;const
char*_tmp7D0;struct Cyc_PP_Doc*_tmp7CF[4];s=((_tmp7CF[3]=Cyc_PP_text(((_tmp7D0=")",
_tag_dyneither(_tmp7D0,sizeof(char),2)))),((_tmp7CF[2]=Cyc_Absynpp_typ2doc(
_tmp281),((_tmp7CF[1]=Cyc_Absynpp_tvars2doc(_tmp280),((_tmp7CF[0]=Cyc_PP_text(((
_tmp7D1="__gen(",_tag_dyneither(_tmp7D1,sizeof(char),7)))),Cyc_PP_cat(
_tag_dyneither(_tmp7CF,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL185;_LL1B8: {
struct Cyc_Absyn_Deref_e_struct*_tmp282=(struct Cyc_Absyn_Deref_e_struct*)_tmp23A;
if(_tmp282->tag != 22)goto _LL1BA;else{_tmp283=_tmp282->f1;}}_LL1B9:{const char*
_tmp7D4;struct Cyc_PP_Doc*_tmp7D3[2];s=((_tmp7D3[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp283),((_tmp7D3[0]=Cyc_PP_text(((_tmp7D4="*",_tag_dyneither(_tmp7D4,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7D3,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL185;_LL1BA: {struct Cyc_Absyn_AggrMember_e_struct*_tmp284=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp23A;if(_tmp284->tag != 23)goto _LL1BC;else{_tmp285=_tmp284->f1;_tmp286=_tmp284->f2;}}
_LL1BB:{const char*_tmp7D7;struct Cyc_PP_Doc*_tmp7D6[3];s=((_tmp7D6[2]=Cyc_PP_textptr(
_tmp286),((_tmp7D6[1]=Cyc_PP_text(((_tmp7D7=".",_tag_dyneither(_tmp7D7,sizeof(
char),2)))),((_tmp7D6[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp285),Cyc_PP_cat(
_tag_dyneither(_tmp7D6,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL185;_LL1BC: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp287=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp23A;if(_tmp287->tag != 24)goto _LL1BE;else{_tmp288=_tmp287->f1;_tmp289=_tmp287->f2;}}
_LL1BD:{const char*_tmp7DA;struct Cyc_PP_Doc*_tmp7D9[3];s=((_tmp7D9[2]=Cyc_PP_textptr(
_tmp289),((_tmp7D9[1]=Cyc_PP_text(((_tmp7DA="->",_tag_dyneither(_tmp7DA,sizeof(
char),3)))),((_tmp7D9[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp288),Cyc_PP_cat(
_tag_dyneither(_tmp7D9,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL185;_LL1BE: {
struct Cyc_Absyn_Subscript_e_struct*_tmp28A=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp23A;if(_tmp28A->tag != 25)goto _LL1C0;else{_tmp28B=_tmp28A->f1;_tmp28C=_tmp28A->f2;}}
_LL1BF:{const char*_tmp7DF;const char*_tmp7DE;struct Cyc_PP_Doc*_tmp7DD[4];s=((
_tmp7DD[3]=Cyc_PP_text(((_tmp7DE="]",_tag_dyneither(_tmp7DE,sizeof(char),2)))),((
_tmp7DD[2]=Cyc_Absynpp_exp2doc(_tmp28C),((_tmp7DD[1]=Cyc_PP_text(((_tmp7DF="[",
_tag_dyneither(_tmp7DF,sizeof(char),2)))),((_tmp7DD[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp28B),Cyc_PP_cat(_tag_dyneither(_tmp7DD,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL185;_LL1C0: {struct Cyc_Absyn_Tuple_e_struct*_tmp28D=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp23A;if(_tmp28D->tag != 26)goto _LL1C2;else{_tmp28E=_tmp28D->f1;}}_LL1C1:{const
char*_tmp7E4;const char*_tmp7E3;struct Cyc_PP_Doc*_tmp7E2[4];s=((_tmp7E2[3]=Cyc_PP_text(((
_tmp7E3=")",_tag_dyneither(_tmp7E3,sizeof(char),2)))),((_tmp7E2[2]=Cyc_Absynpp_exps2doc_prec(
20,_tmp28E),((_tmp7E2[1]=Cyc_PP_text(((_tmp7E4="(",_tag_dyneither(_tmp7E4,
sizeof(char),2)))),((_tmp7E2[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(
_tmp7E2,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL185;_LL1C2: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp28F=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp23A;if(_tmp28F->tag != 27)goto
_LL1C4;else{_tmp290=_tmp28F->f1;_tmp291=_tmp28F->f2;}}_LL1C3:{const char*_tmp7EB;
const char*_tmp7EA;const char*_tmp7E9;struct Cyc_PP_Doc*_tmp7E8[4];s=((_tmp7E8[3]=
Cyc_Absynpp_group_braces(((_tmp7E9=",",_tag_dyneither(_tmp7E9,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp291)),((_tmp7E8[2]=Cyc_PP_text(((_tmp7EA=")",
_tag_dyneither(_tmp7EA,sizeof(char),2)))),((_tmp7E8[1]=Cyc_Absynpp_typ2doc((*
_tmp290).f3),((_tmp7E8[0]=Cyc_PP_text(((_tmp7EB="(",_tag_dyneither(_tmp7EB,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7E8,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL185;_LL1C4: {struct Cyc_Absyn_Array_e_struct*_tmp292=(struct Cyc_Absyn_Array_e_struct*)
_tmp23A;if(_tmp292->tag != 28)goto _LL1C6;else{_tmp293=_tmp292->f1;}}_LL1C5:{const
char*_tmp7EC;s=Cyc_Absynpp_group_braces(((_tmp7EC=",",_tag_dyneither(_tmp7EC,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp293));}goto _LL185;
_LL1C6: {struct Cyc_Absyn_Comprehension_e_struct*_tmp294=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp23A;if(_tmp294->tag != 29)goto _LL1C8;else{_tmp295=_tmp294->f1;_tmp296=_tmp294->f2;
_tmp297=_tmp294->f3;}}_LL1C7:{const char*_tmp7F3;const char*_tmp7F2;const char*
_tmp7F1;struct Cyc_PP_Doc*_tmp7F0[8];s=((_tmp7F0[7]=Cyc_Absynpp_rb(),((_tmp7F0[6]=
Cyc_Absynpp_exp2doc(_tmp297),((_tmp7F0[5]=Cyc_PP_text(((_tmp7F1=" : ",
_tag_dyneither(_tmp7F1,sizeof(char),4)))),((_tmp7F0[4]=Cyc_Absynpp_exp2doc(
_tmp296),((_tmp7F0[3]=Cyc_PP_text(((_tmp7F2=" < ",_tag_dyneither(_tmp7F2,sizeof(
char),4)))),((_tmp7F0[2]=Cyc_PP_text(*(*_tmp295->name).f2),((_tmp7F0[1]=Cyc_PP_text(((
_tmp7F3="for ",_tag_dyneither(_tmp7F3,sizeof(char),5)))),((_tmp7F0[0]=Cyc_Absynpp_lb(),
Cyc_PP_cat(_tag_dyneither(_tmp7F0,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL185;_LL1C8: {struct Cyc_Absyn_Aggregate_e_struct*_tmp298=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp23A;if(_tmp298->tag != 30)goto _LL1CA;else{_tmp299=_tmp298->f1;_tmp29A=_tmp298->f2;
_tmp29B=_tmp298->f3;}}_LL1C9: {struct Cyc_List_List*_tmp30C=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp29B);{const char*_tmp7F8;struct Cyc_List_List*_tmp7F7;struct Cyc_PP_Doc*_tmp7F6[
2];s=((_tmp7F6[1]=Cyc_Absynpp_group_braces(((_tmp7F8=",",_tag_dyneither(_tmp7F8,
sizeof(char),2))),_tmp29A != 0?(_tmp7F7=_cycalloc(sizeof(*_tmp7F7)),((_tmp7F7->hd=
Cyc_Absynpp_tps2doc(_tmp29A),((_tmp7F7->tl=_tmp30C,_tmp7F7))))): _tmp30C),((
_tmp7F6[0]=Cyc_Absynpp_qvar2doc(_tmp299),Cyc_PP_cat(_tag_dyneither(_tmp7F6,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL185;}_LL1CA: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp29C=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp23A;if(_tmp29C->tag != 31)goto
_LL1CC;else{_tmp29D=_tmp29C->f2;}}_LL1CB:{const char*_tmp7F9;s=Cyc_Absynpp_group_braces(((
_tmp7F9=",",_tag_dyneither(_tmp7F9,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp29D));}goto _LL185;_LL1CC: {struct Cyc_Absyn_Datatype_e_struct*_tmp29E=(struct
Cyc_Absyn_Datatype_e_struct*)_tmp23A;if(_tmp29E->tag != 32)goto _LL1CE;else{
_tmp29F=_tmp29E->f1;_tmp2A0=_tmp29E->f3;}}_LL1CD: if(_tmp29F == 0)s=Cyc_Absynpp_qvar2doc(
_tmp2A0->name);else{const char*_tmp800;const char*_tmp7FF;const char*_tmp7FE;struct
Cyc_PP_Doc*_tmp7FD[2];s=((_tmp7FD[1]=Cyc_PP_egroup(((_tmp800="(",_tag_dyneither(
_tmp800,sizeof(char),2))),((_tmp7FF=")",_tag_dyneither(_tmp7FF,sizeof(char),2))),((
_tmp7FE=",",_tag_dyneither(_tmp7FE,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_exp2doc,_tmp29F)),((_tmp7FD[0]=Cyc_Absynpp_qvar2doc(_tmp2A0->name),
Cyc_PP_cat(_tag_dyneither(_tmp7FD,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL185;
_LL1CE: {struct Cyc_Absyn_Enum_e_struct*_tmp2A1=(struct Cyc_Absyn_Enum_e_struct*)
_tmp23A;if(_tmp2A1->tag != 33)goto _LL1D0;else{_tmp2A2=_tmp2A1->f1;}}_LL1CF: s=Cyc_Absynpp_qvar2doc(
_tmp2A2);goto _LL185;_LL1D0: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp2A3=(struct
Cyc_Absyn_AnonEnum_e_struct*)_tmp23A;if(_tmp2A3->tag != 34)goto _LL1D2;else{
_tmp2A4=_tmp2A3->f1;}}_LL1D1: s=Cyc_Absynpp_qvar2doc(_tmp2A4);goto _LL185;_LL1D2: {
struct Cyc_Absyn_Malloc_e_struct*_tmp2A5=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp23A;if(_tmp2A5->tag != 35)goto _LL1D4;else{_tmp2A6=_tmp2A5->f1;_tmp2A7=_tmp2A6.is_calloc;
_tmp2A8=_tmp2A6.rgn;_tmp2A9=_tmp2A6.elt_type;_tmp2AA=_tmp2A6.num_elts;}}_LL1D3:
if(_tmp2A7){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(
_tmp2A9)),0);if(_tmp2A8 == 0){const char*_tmp807;const char*_tmp806;const char*
_tmp805;struct Cyc_PP_Doc*_tmp804[5];s=((_tmp804[4]=Cyc_PP_text(((_tmp805=")",
_tag_dyneither(_tmp805,sizeof(char),2)))),((_tmp804[3]=Cyc_Absynpp_exp2doc(st),((
_tmp804[2]=Cyc_PP_text(((_tmp806=",",_tag_dyneither(_tmp806,sizeof(char),2)))),((
_tmp804[1]=Cyc_Absynpp_exp2doc(_tmp2AA),((_tmp804[0]=Cyc_PP_text(((_tmp807="calloc(",
_tag_dyneither(_tmp807,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp804,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{const char*_tmp810;const char*
_tmp80F;const char*_tmp80E;const char*_tmp80D;struct Cyc_PP_Doc*_tmp80C[7];s=((
_tmp80C[6]=Cyc_PP_text(((_tmp80D=")",_tag_dyneither(_tmp80D,sizeof(char),2)))),((
_tmp80C[5]=Cyc_Absynpp_exp2doc(st),((_tmp80C[4]=Cyc_PP_text(((_tmp80E=",",
_tag_dyneither(_tmp80E,sizeof(char),2)))),((_tmp80C[3]=Cyc_Absynpp_exp2doc(
_tmp2AA),((_tmp80C[2]=Cyc_PP_text(((_tmp80F=",",_tag_dyneither(_tmp80F,sizeof(
char),2)))),((_tmp80C[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp2A8),((
_tmp80C[0]=Cyc_PP_text(((_tmp810="rcalloc(",_tag_dyneither(_tmp810,sizeof(char),
9)))),Cyc_PP_cat(_tag_dyneither(_tmp80C,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}
else{struct Cyc_Absyn_Exp*new_e;if(_tmp2A9 == 0)new_e=_tmp2AA;else{new_e=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(*_tmp2A9,0),_tmp2AA,0);}if(_tmp2A8 == 0){const char*
_tmp815;const char*_tmp814;struct Cyc_PP_Doc*_tmp813[3];s=((_tmp813[2]=Cyc_PP_text(((
_tmp814=")",_tag_dyneither(_tmp814,sizeof(char),2)))),((_tmp813[1]=Cyc_Absynpp_exp2doc(
new_e),((_tmp813[0]=Cyc_PP_text(((_tmp815="malloc(",_tag_dyneither(_tmp815,
sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp813,sizeof(struct Cyc_PP_Doc*),3))))))));}
else{const char*_tmp81C;const char*_tmp81B;const char*_tmp81A;struct Cyc_PP_Doc*
_tmp819[5];s=((_tmp819[4]=Cyc_PP_text(((_tmp81A=")",_tag_dyneither(_tmp81A,
sizeof(char),2)))),((_tmp819[3]=Cyc_Absynpp_exp2doc(new_e),((_tmp819[2]=Cyc_PP_text(((
_tmp81B=",",_tag_dyneither(_tmp81B,sizeof(char),2)))),((_tmp819[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp2A8),((_tmp819[0]=Cyc_PP_text(((_tmp81C="rmalloc(",
_tag_dyneither(_tmp81C,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp819,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}}goto _LL185;_LL1D4: {struct Cyc_Absyn_Swap_e_struct*
_tmp2AB=(struct Cyc_Absyn_Swap_e_struct*)_tmp23A;if(_tmp2AB->tag != 36)goto _LL1D6;
else{_tmp2AC=_tmp2AB->f1;_tmp2AD=_tmp2AB->f2;}}_LL1D5:{const char*_tmp823;const
char*_tmp822;const char*_tmp821;struct Cyc_PP_Doc*_tmp820[5];s=((_tmp820[4]=Cyc_PP_text(((
_tmp821=")",_tag_dyneither(_tmp821,sizeof(char),2)))),((_tmp820[3]=Cyc_Absynpp_exp2doc(
_tmp2AD),((_tmp820[2]=Cyc_PP_text(((_tmp822=",",_tag_dyneither(_tmp822,sizeof(
char),2)))),((_tmp820[1]=Cyc_Absynpp_exp2doc(_tmp2AC),((_tmp820[0]=Cyc_PP_text(((
_tmp823="cycswap(",_tag_dyneither(_tmp823,sizeof(char),9)))),Cyc_PP_cat(
_tag_dyneither(_tmp820,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL185;
_LL1D6: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp2AE=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp23A;if(_tmp2AE->tag != 37)goto _LL1D8;else{_tmp2AF=_tmp2AE->f1;_tmp2B0=_tmp2AE->f2;}}
_LL1D7:{const char*_tmp824;s=Cyc_Absynpp_group_braces(((_tmp824=",",
_tag_dyneither(_tmp824,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp2B0));}goto _LL185;_LL1D8: {struct Cyc_Absyn_StmtExp_e_struct*_tmp2B1=(struct
Cyc_Absyn_StmtExp_e_struct*)_tmp23A;if(_tmp2B1->tag != 38)goto _LL185;else{_tmp2B2=
_tmp2B1->f1;}}_LL1D9:{const char*_tmp829;const char*_tmp828;struct Cyc_PP_Doc*
_tmp827[7];s=((_tmp827[6]=Cyc_PP_text(((_tmp828=")",_tag_dyneither(_tmp828,
sizeof(char),2)))),((_tmp827[5]=Cyc_Absynpp_rb(),((_tmp827[4]=Cyc_PP_blank_doc(),((
_tmp827[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2B2,1)),((_tmp827[2]=Cyc_PP_blank_doc(),((
_tmp827[1]=Cyc_Absynpp_lb(),((_tmp827[0]=Cyc_PP_text(((_tmp829="(",
_tag_dyneither(_tmp829,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp827,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}goto _LL185;_LL185:;}if(inprec >= 
myprec){const char*_tmp82E;const char*_tmp82D;struct Cyc_PP_Doc*_tmp82C[3];s=((
_tmp82C[2]=Cyc_PP_text(((_tmp82D=")",_tag_dyneither(_tmp82D,sizeof(char),2)))),((
_tmp82C[1]=s,((_tmp82C[0]=Cyc_PP_text(((_tmp82E="(",_tag_dyneither(_tmp82E,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp82C,sizeof(struct Cyc_PP_Doc*),3))))))));}
return s;}struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d);struct Cyc_PP_Doc*
Cyc_Absynpp_designator2doc(void*d){void*_tmp330=d;struct Cyc_Absyn_Exp*_tmp332;
struct _dyneither_ptr*_tmp334;_LL1E0: {struct Cyc_Absyn_ArrayElement_struct*
_tmp331=(struct Cyc_Absyn_ArrayElement_struct*)_tmp330;if(_tmp331->tag != 0)goto
_LL1E2;else{_tmp332=_tmp331->f1;}}_LL1E1: {const char*_tmp833;const char*_tmp832;
struct Cyc_PP_Doc*_tmp831[3];return(_tmp831[2]=Cyc_PP_text(((_tmp832="]",
_tag_dyneither(_tmp832,sizeof(char),2)))),((_tmp831[1]=Cyc_Absynpp_exp2doc(
_tmp332),((_tmp831[0]=Cyc_PP_text(((_tmp833=".[",_tag_dyneither(_tmp833,sizeof(
char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp831,sizeof(struct Cyc_PP_Doc*),3)))))));}
_LL1E2: {struct Cyc_Absyn_FieldName_struct*_tmp333=(struct Cyc_Absyn_FieldName_struct*)
_tmp330;if(_tmp333->tag != 1)goto _LL1DF;else{_tmp334=_tmp333->f1;}}_LL1E3: {const
char*_tmp836;struct Cyc_PP_Doc*_tmp835[2];return(_tmp835[1]=Cyc_PP_textptr(
_tmp334),((_tmp835[0]=Cyc_PP_text(((_tmp836=".",_tag_dyneither(_tmp836,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp835,sizeof(struct Cyc_PP_Doc*),2)))));}
_LL1DF:;}struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple15*de);struct Cyc_PP_Doc*
Cyc_Absynpp_de2doc(struct _tuple15*de){if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*
de).f2);else{const char*_tmp83D;const char*_tmp83C;const char*_tmp83B;struct Cyc_PP_Doc*
_tmp83A[2];return(_tmp83A[1]=Cyc_Absynpp_exp2doc((*de).f2),((_tmp83A[0]=Cyc_PP_egroup(((
_tmp83D="",_tag_dyneither(_tmp83D,sizeof(char),1))),((_tmp83C="=",_tag_dyneither(
_tmp83C,sizeof(char),2))),((_tmp83B="=",_tag_dyneither(_tmp83B,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*de).f1)),Cyc_PP_cat(_tag_dyneither(_tmp83A,sizeof(
struct Cyc_PP_Doc*),2)))));}}struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,
struct Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,
struct Cyc_List_List*es){const char*_tmp840;const char*_tmp83F;const char*_tmp83E;
return Cyc_PP_group(((_tmp83E="",_tag_dyneither(_tmp83E,sizeof(char),1))),((
_tmp83F="",_tag_dyneither(_tmp83F,sizeof(char),1))),((_tmp840=",",_tag_dyneither(
_tmp840,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,
struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,
inprec,es));}struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c);struct
Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){union Cyc_Absyn_Cnst _tmp341=
c;struct _tuple3 _tmp342;enum Cyc_Absyn_Sign _tmp343;char _tmp344;struct _tuple4
_tmp345;enum Cyc_Absyn_Sign _tmp346;short _tmp347;struct _tuple5 _tmp348;enum Cyc_Absyn_Sign
_tmp349;int _tmp34A;struct _tuple5 _tmp34B;enum Cyc_Absyn_Sign _tmp34C;int _tmp34D;
struct _tuple5 _tmp34E;enum Cyc_Absyn_Sign _tmp34F;int _tmp350;struct _tuple6 _tmp351;
enum Cyc_Absyn_Sign _tmp352;long long _tmp353;struct _dyneither_ptr _tmp354;int
_tmp355;struct _dyneither_ptr _tmp356;_LL1E5: if((_tmp341.Char_c).tag != 2)goto
_LL1E7;_tmp342=(struct _tuple3)(_tmp341.Char_c).val;_tmp343=_tmp342.f1;_tmp344=
_tmp342.f2;_LL1E6: {const char*_tmp844;void*_tmp843[1];struct Cyc_String_pa_struct
_tmp842;return Cyc_PP_text((struct _dyneither_ptr)((_tmp842.tag=0,((_tmp842.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp344)),((
_tmp843[0]=& _tmp842,Cyc_aprintf(((_tmp844="'%s'",_tag_dyneither(_tmp844,sizeof(
char),5))),_tag_dyneither(_tmp843,sizeof(void*),1)))))))));}_LL1E7: if((_tmp341.Short_c).tag
!= 3)goto _LL1E9;_tmp345=(struct _tuple4)(_tmp341.Short_c).val;_tmp346=_tmp345.f1;
_tmp347=_tmp345.f2;_LL1E8: {const char*_tmp848;void*_tmp847[1];struct Cyc_Int_pa_struct
_tmp846;return Cyc_PP_text((struct _dyneither_ptr)((_tmp846.tag=1,((_tmp846.f1=(
unsigned long)((int)_tmp347),((_tmp847[0]=& _tmp846,Cyc_aprintf(((_tmp848="%d",
_tag_dyneither(_tmp848,sizeof(char),3))),_tag_dyneither(_tmp847,sizeof(void*),1)))))))));}
_LL1E9: if((_tmp341.Int_c).tag != 4)goto _LL1EB;_tmp348=(struct _tuple5)(_tmp341.Int_c).val;
_tmp349=_tmp348.f1;if(_tmp349 != Cyc_Absyn_None)goto _LL1EB;_tmp34A=_tmp348.f2;
_LL1EA: _tmp34D=_tmp34A;goto _LL1EC;_LL1EB: if((_tmp341.Int_c).tag != 4)goto _LL1ED;
_tmp34B=(struct _tuple5)(_tmp341.Int_c).val;_tmp34C=_tmp34B.f1;if(_tmp34C != Cyc_Absyn_Signed)
goto _LL1ED;_tmp34D=_tmp34B.f2;_LL1EC: {const char*_tmp84C;void*_tmp84B[1];struct
Cyc_Int_pa_struct _tmp84A;return Cyc_PP_text((struct _dyneither_ptr)((_tmp84A.tag=1,((
_tmp84A.f1=(unsigned long)_tmp34D,((_tmp84B[0]=& _tmp84A,Cyc_aprintf(((_tmp84C="%d",
_tag_dyneither(_tmp84C,sizeof(char),3))),_tag_dyneither(_tmp84B,sizeof(void*),1)))))))));}
_LL1ED: if((_tmp341.Int_c).tag != 4)goto _LL1EF;_tmp34E=(struct _tuple5)(_tmp341.Int_c).val;
_tmp34F=_tmp34E.f1;if(_tmp34F != Cyc_Absyn_Unsigned)goto _LL1EF;_tmp350=_tmp34E.f2;
_LL1EE: {const char*_tmp850;void*_tmp84F[1];struct Cyc_Int_pa_struct _tmp84E;return
Cyc_PP_text((struct _dyneither_ptr)((_tmp84E.tag=1,((_tmp84E.f1=(unsigned int)
_tmp350,((_tmp84F[0]=& _tmp84E,Cyc_aprintf(((_tmp850="%u",_tag_dyneither(_tmp850,
sizeof(char),3))),_tag_dyneither(_tmp84F,sizeof(void*),1)))))))));}_LL1EF: if((
_tmp341.LongLong_c).tag != 5)goto _LL1F1;_tmp351=(struct _tuple6)(_tmp341.LongLong_c).val;
_tmp352=_tmp351.f1;_tmp353=_tmp351.f2;_LL1F0: {const char*_tmp851;return Cyc_PP_text(((
_tmp851="<<FIX LONG LONG CONSTANT>>",_tag_dyneither(_tmp851,sizeof(char),27))));}
_LL1F1: if((_tmp341.Float_c).tag != 6)goto _LL1F3;_tmp354=(struct _dyneither_ptr)(
_tmp341.Float_c).val;_LL1F2: return Cyc_PP_text(_tmp354);_LL1F3: if((_tmp341.Null_c).tag
!= 1)goto _LL1F5;_tmp355=(int)(_tmp341.Null_c).val;_LL1F4: {const char*_tmp852;
return Cyc_PP_text(((_tmp852="NULL",_tag_dyneither(_tmp852,sizeof(char),5))));}
_LL1F5: if((_tmp341.String_c).tag != 7)goto _LL1E4;_tmp356=(struct _dyneither_ptr)(
_tmp341.String_c).val;_LL1F6: {const char*_tmp857;const char*_tmp856;struct Cyc_PP_Doc*
_tmp855[3];return(_tmp855[2]=Cyc_PP_text(((_tmp856="\"",_tag_dyneither(_tmp856,
sizeof(char),2)))),((_tmp855[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp356)),((
_tmp855[0]=Cyc_PP_text(((_tmp857="\"",_tag_dyneither(_tmp857,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp855,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL1E4:;}
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct
Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop
p,struct Cyc_List_List*es){struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);if(p == (
enum Cyc_Absyn_Primop)Cyc_Absyn_Numelts){if(es == 0  || es->tl != 0){struct Cyc_Core_Failure_struct
_tmp864;const char*_tmp863;void*_tmp862[1];struct Cyc_String_pa_struct _tmp861;
struct Cyc_Core_Failure_struct*_tmp860;(int)_throw((void*)((_tmp860=_cycalloc(
sizeof(*_tmp860)),((_tmp860[0]=((_tmp864.tag=Cyc_Core_Failure,((_tmp864.f1=(
struct _dyneither_ptr)((_tmp861.tag=0,((_tmp861.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp862[0]=& _tmp861,Cyc_aprintf(((
_tmp863="Absynpp::primapp2doc Numelts: %s with bad args",_tag_dyneither(_tmp863,
sizeof(char),47))),_tag_dyneither(_tmp862,sizeof(void*),1)))))))),_tmp864)))),
_tmp860)))));}{const char*_tmp869;const char*_tmp868;struct Cyc_PP_Doc*_tmp867[3];
return(_tmp867[2]=Cyc_PP_text(((_tmp868=")",_tag_dyneither(_tmp868,sizeof(char),
2)))),((_tmp867[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd),((_tmp867[0]=
Cyc_PP_text(((_tmp869="numelts(",_tag_dyneither(_tmp869,sizeof(char),9)))),Cyc_PP_cat(
_tag_dyneither(_tmp867,sizeof(struct Cyc_PP_Doc*),3)))))));};}else{struct Cyc_List_List*
ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0){struct Cyc_Core_Failure_struct _tmp876;const char*_tmp875;void*_tmp874[1];
struct Cyc_String_pa_struct _tmp873;struct Cyc_Core_Failure_struct*_tmp872;(int)
_throw((void*)((_tmp872=_cycalloc(sizeof(*_tmp872)),((_tmp872[0]=((_tmp876.tag=
Cyc_Core_Failure,((_tmp876.f1=(struct _dyneither_ptr)((_tmp873.tag=0,((_tmp873.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((
_tmp874[0]=& _tmp873,Cyc_aprintf(((_tmp875="Absynpp::primapp2doc: %s with no args",
_tag_dyneither(_tmp875,sizeof(char),38))),_tag_dyneither(_tmp874,sizeof(void*),1)))))))),
_tmp876)))),_tmp872)))));}else{if(ds->tl == 0){const char*_tmp879;struct Cyc_PP_Doc*
_tmp878[3];return(_tmp878[2]=(struct Cyc_PP_Doc*)ds->hd,((_tmp878[1]=Cyc_PP_text(((
_tmp879=" ",_tag_dyneither(_tmp879,sizeof(char),2)))),((_tmp878[0]=ps,Cyc_PP_cat(
_tag_dyneither(_tmp878,sizeof(struct Cyc_PP_Doc*),3)))))));}else{if(((struct Cyc_List_List*)
_check_null(ds->tl))->tl != 0){struct Cyc_Core_Failure_struct _tmp886;const char*
_tmp885;void*_tmp884[1];struct Cyc_String_pa_struct _tmp883;struct Cyc_Core_Failure_struct*
_tmp882;(int)_throw((void*)((_tmp882=_cycalloc(sizeof(*_tmp882)),((_tmp882[0]=((
_tmp886.tag=Cyc_Core_Failure,((_tmp886.f1=(struct _dyneither_ptr)((_tmp883.tag=0,((
_tmp883.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,
72)),((_tmp884[0]=& _tmp883,Cyc_aprintf(((_tmp885="Absynpp::primapp2doc: %s with more than 2 args",
_tag_dyneither(_tmp885,sizeof(char),47))),_tag_dyneither(_tmp884,sizeof(void*),1)))))))),
_tmp886)))),_tmp882)))));}else{const char*_tmp88B;const char*_tmp88A;struct Cyc_PP_Doc*
_tmp889[5];return(_tmp889[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)
_check_null(ds->tl))->hd,((_tmp889[3]=Cyc_PP_text(((_tmp88A=" ",_tag_dyneither(
_tmp88A,sizeof(char),2)))),((_tmp889[2]=ps,((_tmp889[1]=Cyc_PP_text(((_tmp88B=" ",
_tag_dyneither(_tmp88B,sizeof(char),2)))),((_tmp889[0]=(struct Cyc_PP_Doc*)ds->hd,
Cyc_PP_cat(_tag_dyneither(_tmp889,sizeof(struct Cyc_PP_Doc*),5)))))))))));}}}}}
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p);struct
_dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){switch(p){case Cyc_Absyn_Plus:
_LL1F7: {const char*_tmp88C;return(_tmp88C="+",_tag_dyneither(_tmp88C,sizeof(char),
2));}case Cyc_Absyn_Times: _LL1F8: {const char*_tmp88D;return(_tmp88D="*",
_tag_dyneither(_tmp88D,sizeof(char),2));}case Cyc_Absyn_Minus: _LL1F9: {const char*
_tmp88E;return(_tmp88E="-",_tag_dyneither(_tmp88E,sizeof(char),2));}case Cyc_Absyn_Div:
_LL1FA: {const char*_tmp88F;return(_tmp88F="/",_tag_dyneither(_tmp88F,sizeof(char),
2));}case Cyc_Absyn_Mod: _LL1FB: {const char*_tmp891;const char*_tmp890;return Cyc_Absynpp_print_for_cycdoc?(
_tmp891="\\%",_tag_dyneither(_tmp891,sizeof(char),3)):((_tmp890="%",
_tag_dyneither(_tmp890,sizeof(char),2)));}case Cyc_Absyn_Eq: _LL1FC: {const char*
_tmp892;return(_tmp892="==",_tag_dyneither(_tmp892,sizeof(char),3));}case Cyc_Absyn_Neq:
_LL1FD: {const char*_tmp893;return(_tmp893="!=",_tag_dyneither(_tmp893,sizeof(
char),3));}case Cyc_Absyn_Gt: _LL1FE: {const char*_tmp894;return(_tmp894=">",
_tag_dyneither(_tmp894,sizeof(char),2));}case Cyc_Absyn_Lt: _LL1FF: {const char*
_tmp895;return(_tmp895="<",_tag_dyneither(_tmp895,sizeof(char),2));}case Cyc_Absyn_Gte:
_LL200: {const char*_tmp896;return(_tmp896=">=",_tag_dyneither(_tmp896,sizeof(
char),3));}case Cyc_Absyn_Lte: _LL201: {const char*_tmp897;return(_tmp897="<=",
_tag_dyneither(_tmp897,sizeof(char),3));}case Cyc_Absyn_Not: _LL202: {const char*
_tmp898;return(_tmp898="!",_tag_dyneither(_tmp898,sizeof(char),2));}case Cyc_Absyn_Bitnot:
_LL203: {const char*_tmp899;return(_tmp899="~",_tag_dyneither(_tmp899,sizeof(char),
2));}case Cyc_Absyn_Bitand: _LL204: {const char*_tmp89A;return(_tmp89A="&",
_tag_dyneither(_tmp89A,sizeof(char),2));}case Cyc_Absyn_Bitor: _LL205: {const char*
_tmp89B;return(_tmp89B="|",_tag_dyneither(_tmp89B,sizeof(char),2));}case Cyc_Absyn_Bitxor:
_LL206: {const char*_tmp89C;return(_tmp89C="^",_tag_dyneither(_tmp89C,sizeof(char),
2));}case Cyc_Absyn_Bitlshift: _LL207: {const char*_tmp89D;return(_tmp89D="<<",
_tag_dyneither(_tmp89D,sizeof(char),3));}case Cyc_Absyn_Bitlrshift: _LL208: {const
char*_tmp89E;return(_tmp89E=">>",_tag_dyneither(_tmp89E,sizeof(char),3));}case
Cyc_Absyn_Bitarshift: _LL209: {const char*_tmp89F;return(_tmp89F=">>>",
_tag_dyneither(_tmp89F,sizeof(char),4));}case Cyc_Absyn_Numelts: _LL20A: {const
char*_tmp8A0;return(_tmp8A0="numelts",_tag_dyneither(_tmp8A0,sizeof(char),8));}}}
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p);struct Cyc_PP_Doc*
Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){return Cyc_PP_text(Cyc_Absynpp_prim2str(
p));}int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s){void*_tmp394=s->r;_LL20D: {struct Cyc_Absyn_Decl_s_struct*
_tmp395=(struct Cyc_Absyn_Decl_s_struct*)_tmp394;if(_tmp395->tag != 12)goto _LL20F;}
_LL20E: return 1;_LL20F:;_LL210: return 0;_LL20C:;}struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(
struct Cyc_Absyn_Stmt*st,int stmtexp);struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct
Cyc_Absyn_Stmt*st,int stmtexp){struct Cyc_PP_Doc*s;{void*_tmp396=st->r;struct Cyc_Absyn_Exp*
_tmp399;struct Cyc_Absyn_Stmt*_tmp39B;struct Cyc_Absyn_Stmt*_tmp39C;struct Cyc_Absyn_Exp*
_tmp39E;struct Cyc_Absyn_Exp*_tmp3A0;struct Cyc_Absyn_Stmt*_tmp3A1;struct Cyc_Absyn_Stmt*
_tmp3A2;struct _tuple8 _tmp3A4;struct Cyc_Absyn_Exp*_tmp3A5;struct Cyc_Absyn_Stmt*
_tmp3A6;struct _dyneither_ptr*_tmp3AA;struct Cyc_Absyn_Exp*_tmp3AC;struct _tuple8
_tmp3AD;struct Cyc_Absyn_Exp*_tmp3AE;struct _tuple8 _tmp3AF;struct Cyc_Absyn_Exp*
_tmp3B0;struct Cyc_Absyn_Stmt*_tmp3B1;struct Cyc_Absyn_Exp*_tmp3B3;struct Cyc_List_List*
_tmp3B4;struct Cyc_List_List*_tmp3B6;struct Cyc_List_List*_tmp3B8;struct Cyc_Absyn_Decl*
_tmp3BA;struct Cyc_Absyn_Stmt*_tmp3BB;struct _dyneither_ptr*_tmp3BD;struct Cyc_Absyn_Stmt*
_tmp3BE;struct Cyc_Absyn_Stmt*_tmp3C0;struct _tuple8 _tmp3C1;struct Cyc_Absyn_Exp*
_tmp3C2;struct Cyc_Absyn_Stmt*_tmp3C4;struct Cyc_List_List*_tmp3C5;struct Cyc_Absyn_Exp*
_tmp3C7;_LL212: {struct Cyc_Absyn_Skip_s_struct*_tmp397=(struct Cyc_Absyn_Skip_s_struct*)
_tmp396;if(_tmp397->tag != 0)goto _LL214;}_LL213:{const char*_tmp8A1;s=Cyc_PP_text(((
_tmp8A1=";",_tag_dyneither(_tmp8A1,sizeof(char),2))));}goto _LL211;_LL214: {
struct Cyc_Absyn_Exp_s_struct*_tmp398=(struct Cyc_Absyn_Exp_s_struct*)_tmp396;if(
_tmp398->tag != 1)goto _LL216;else{_tmp399=_tmp398->f1;}}_LL215:{const char*_tmp8A4;
struct Cyc_PP_Doc*_tmp8A3[2];s=((_tmp8A3[1]=Cyc_PP_text(((_tmp8A4=";",
_tag_dyneither(_tmp8A4,sizeof(char),2)))),((_tmp8A3[0]=Cyc_Absynpp_exp2doc(
_tmp399),Cyc_PP_cat(_tag_dyneither(_tmp8A3,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL211;_LL216: {struct Cyc_Absyn_Seq_s_struct*_tmp39A=(struct Cyc_Absyn_Seq_s_struct*)
_tmp396;if(_tmp39A->tag != 2)goto _LL218;else{_tmp39B=_tmp39A->f1;_tmp39C=_tmp39A->f2;}}
_LL217: if(Cyc_Absynpp_decls_first){if(Cyc_Absynpp_is_declaration(_tmp39B)){
struct Cyc_PP_Doc*_tmp8AF[5];struct Cyc_PP_Doc*_tmp8AE[7];const char*_tmp8AD;const
char*_tmp8AC;struct Cyc_PP_Doc*_tmp8AB[7];s=((_tmp8AB[6]=Cyc_Absynpp_is_declaration(
_tmp39C)?stmtexp?(_tmp8AE[6]=Cyc_PP_line_doc(),((_tmp8AE[5]=Cyc_PP_text(((
_tmp8AD=");",_tag_dyneither(_tmp8AD,sizeof(char),3)))),((_tmp8AE[4]=Cyc_Absynpp_rb(),((
_tmp8AE[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp39C,stmtexp)),((_tmp8AE[2]=Cyc_PP_blank_doc(),((
_tmp8AE[1]=Cyc_Absynpp_lb(),((_tmp8AE[0]=Cyc_PP_text(((_tmp8AC="(",
_tag_dyneither(_tmp8AC,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8AE,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))):((_tmp8AF[4]=Cyc_PP_line_doc(),((
_tmp8AF[3]=Cyc_Absynpp_rb(),((_tmp8AF[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp39C,stmtexp)),((_tmp8AF[1]=Cyc_PP_blank_doc(),((_tmp8AF[0]=Cyc_Absynpp_lb(),
Cyc_PP_cat(_tag_dyneither(_tmp8AF,sizeof(struct Cyc_PP_Doc*),5)))))))))))): Cyc_Absynpp_stmt2doc(
_tmp39C,stmtexp),((_tmp8AB[5]=Cyc_PP_line_doc(),((_tmp8AB[4]=Cyc_Absynpp_rb(),((
_tmp8AB[3]=Cyc_PP_line_doc(),((_tmp8AB[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp39B,0)),((_tmp8AB[1]=Cyc_PP_blank_doc(),((_tmp8AB[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(
_tag_dyneither(_tmp8AB,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{if(Cyc_Absynpp_is_declaration(
_tmp39C)){struct Cyc_PP_Doc*_tmp8BA[4];struct Cyc_PP_Doc*_tmp8B9[6];const char*
_tmp8B8;const char*_tmp8B7;struct Cyc_PP_Doc*_tmp8B6[4];s=((_tmp8B6[3]=Cyc_PP_line_doc(),((
_tmp8B6[2]=stmtexp?(_tmp8B9[5]=Cyc_PP_text(((_tmp8B8=");",_tag_dyneither(_tmp8B8,
sizeof(char),3)))),((_tmp8B9[4]=Cyc_Absynpp_rb(),((_tmp8B9[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp39C,stmtexp)),((_tmp8B9[2]=Cyc_PP_blank_doc(),((_tmp8B9[1]=Cyc_Absynpp_lb(),((
_tmp8B9[0]=Cyc_PP_text(((_tmp8B7="(",_tag_dyneither(_tmp8B7,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp8B9,sizeof(struct Cyc_PP_Doc*),6))))))))))))):((
_tmp8BA[3]=Cyc_Absynpp_rb(),((_tmp8BA[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp39C,stmtexp)),((_tmp8BA[1]=Cyc_PP_blank_doc(),((_tmp8BA[0]=Cyc_Absynpp_lb(),
Cyc_PP_cat(_tag_dyneither(_tmp8BA,sizeof(struct Cyc_PP_Doc*),4)))))))))),((
_tmp8B6[1]=Cyc_PP_line_doc(),((_tmp8B6[0]=Cyc_Absynpp_stmt2doc(_tmp39B,0),Cyc_PP_cat(
_tag_dyneither(_tmp8B6,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{struct Cyc_PP_Doc*
_tmp8BB[3];s=((_tmp8BB[2]=Cyc_Absynpp_stmt2doc(_tmp39C,stmtexp),((_tmp8BB[1]=Cyc_PP_line_doc(),((
_tmp8BB[0]=Cyc_Absynpp_stmt2doc(_tmp39B,0),Cyc_PP_cat(_tag_dyneither(_tmp8BB,
sizeof(struct Cyc_PP_Doc*),3))))))));}}}else{struct Cyc_PP_Doc*_tmp8BC[3];s=((
_tmp8BC[2]=Cyc_Absynpp_stmt2doc(_tmp39C,stmtexp),((_tmp8BC[1]=Cyc_PP_line_doc(),((
_tmp8BC[0]=Cyc_Absynpp_stmt2doc(_tmp39B,0),Cyc_PP_cat(_tag_dyneither(_tmp8BC,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL211;_LL218: {struct Cyc_Absyn_Return_s_struct*
_tmp39D=(struct Cyc_Absyn_Return_s_struct*)_tmp396;if(_tmp39D->tag != 3)goto _LL21A;
else{_tmp39E=_tmp39D->f1;}}_LL219: if(_tmp39E == 0){const char*_tmp8BD;s=Cyc_PP_text(((
_tmp8BD="return;",_tag_dyneither(_tmp8BD,sizeof(char),8))));}else{const char*
_tmp8C2;const char*_tmp8C1;struct Cyc_PP_Doc*_tmp8C0[3];s=((_tmp8C0[2]=Cyc_PP_text(((
_tmp8C1=";",_tag_dyneither(_tmp8C1,sizeof(char),2)))),((_tmp8C0[1]=_tmp39E == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp39E),((_tmp8C0[0]=
Cyc_PP_text(((_tmp8C2="return ",_tag_dyneither(_tmp8C2,sizeof(char),8)))),Cyc_PP_cat(
_tag_dyneither(_tmp8C0,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL211;_LL21A: {
struct Cyc_Absyn_IfThenElse_s_struct*_tmp39F=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp396;if(_tmp39F->tag != 4)goto _LL21C;else{_tmp3A0=_tmp39F->f1;_tmp3A1=_tmp39F->f2;
_tmp3A2=_tmp39F->f3;}}_LL21B: {int print_else;{void*_tmp3DB=_tmp3A2->r;_LL237: {
struct Cyc_Absyn_Skip_s_struct*_tmp3DC=(struct Cyc_Absyn_Skip_s_struct*)_tmp3DB;
if(_tmp3DC->tag != 0)goto _LL239;}_LL238: print_else=0;goto _LL236;_LL239:;_LL23A:
print_else=1;goto _LL236;_LL236:;}{const char*_tmp8D1;const char*_tmp8D0;struct Cyc_PP_Doc*
_tmp8CF[2];struct Cyc_PP_Doc*_tmp8CE[6];struct Cyc_PP_Doc*_tmp8CD[2];const char*
_tmp8CC;struct Cyc_PP_Doc*_tmp8CB[8];s=((_tmp8CB[7]=print_else?(_tmp8CE[5]=Cyc_Absynpp_rb(),((
_tmp8CE[4]=Cyc_PP_line_doc(),((_tmp8CE[3]=Cyc_PP_nest(2,((_tmp8CD[1]=Cyc_Absynpp_stmt2doc(
_tmp3A2,0),((_tmp8CD[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8CD,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8CE[2]=Cyc_Absynpp_lb(),((_tmp8CE[1]=
Cyc_PP_text(((_tmp8CC="else ",_tag_dyneither(_tmp8CC,sizeof(char),6)))),((
_tmp8CE[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8CE,sizeof(struct Cyc_PP_Doc*),
6))))))))))))): Cyc_PP_nil_doc(),((_tmp8CB[6]=Cyc_Absynpp_rb(),((_tmp8CB[5]=Cyc_PP_line_doc(),((
_tmp8CB[4]=Cyc_PP_nest(2,((_tmp8CF[1]=Cyc_Absynpp_stmt2doc(_tmp3A1,0),((_tmp8CF[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8CF,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp8CB[3]=Cyc_Absynpp_lb(),((_tmp8CB[2]=Cyc_PP_text(((_tmp8D0=") ",
_tag_dyneither(_tmp8D0,sizeof(char),3)))),((_tmp8CB[1]=Cyc_Absynpp_exp2doc(
_tmp3A0),((_tmp8CB[0]=Cyc_PP_text(((_tmp8D1="if (",_tag_dyneither(_tmp8D1,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8CB,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL211;}_LL21C: {struct Cyc_Absyn_While_s_struct*_tmp3A3=(struct Cyc_Absyn_While_s_struct*)
_tmp396;if(_tmp3A3->tag != 5)goto _LL21E;else{_tmp3A4=_tmp3A3->f1;_tmp3A5=_tmp3A4.f1;
_tmp3A6=_tmp3A3->f2;}}_LL21D:{const char*_tmp8D8;const char*_tmp8D7;struct Cyc_PP_Doc*
_tmp8D6[2];struct Cyc_PP_Doc*_tmp8D5[7];s=((_tmp8D5[6]=Cyc_Absynpp_rb(),((_tmp8D5[
5]=Cyc_PP_line_doc(),((_tmp8D5[4]=Cyc_PP_nest(2,((_tmp8D6[1]=Cyc_Absynpp_stmt2doc(
_tmp3A6,0),((_tmp8D6[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8D6,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8D5[3]=Cyc_Absynpp_lb(),((_tmp8D5[2]=
Cyc_PP_text(((_tmp8D7=") ",_tag_dyneither(_tmp8D7,sizeof(char),3)))),((_tmp8D5[1]=
Cyc_Absynpp_exp2doc(_tmp3A5),((_tmp8D5[0]=Cyc_PP_text(((_tmp8D8="while (",
_tag_dyneither(_tmp8D8,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp8D5,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}goto _LL211;_LL21E: {struct Cyc_Absyn_Break_s_struct*
_tmp3A7=(struct Cyc_Absyn_Break_s_struct*)_tmp396;if(_tmp3A7->tag != 6)goto _LL220;}
_LL21F:{const char*_tmp8D9;s=Cyc_PP_text(((_tmp8D9="break;",_tag_dyneither(
_tmp8D9,sizeof(char),7))));}goto _LL211;_LL220: {struct Cyc_Absyn_Continue_s_struct*
_tmp3A8=(struct Cyc_Absyn_Continue_s_struct*)_tmp396;if(_tmp3A8->tag != 7)goto
_LL222;}_LL221:{const char*_tmp8DA;s=Cyc_PP_text(((_tmp8DA="continue;",
_tag_dyneither(_tmp8DA,sizeof(char),10))));}goto _LL211;_LL222: {struct Cyc_Absyn_Goto_s_struct*
_tmp3A9=(struct Cyc_Absyn_Goto_s_struct*)_tmp396;if(_tmp3A9->tag != 8)goto _LL224;
else{_tmp3AA=_tmp3A9->f1;}}_LL223:{const char*_tmp8DE;void*_tmp8DD[1];struct Cyc_String_pa_struct
_tmp8DC;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp8DC.tag=0,((_tmp8DC.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp3AA),((_tmp8DD[0]=& _tmp8DC,Cyc_aprintf(((
_tmp8DE="goto %s;",_tag_dyneither(_tmp8DE,sizeof(char),9))),_tag_dyneither(
_tmp8DD,sizeof(void*),1)))))))));}goto _LL211;_LL224: {struct Cyc_Absyn_For_s_struct*
_tmp3AB=(struct Cyc_Absyn_For_s_struct*)_tmp396;if(_tmp3AB->tag != 9)goto _LL226;
else{_tmp3AC=_tmp3AB->f1;_tmp3AD=_tmp3AB->f2;_tmp3AE=_tmp3AD.f1;_tmp3AF=_tmp3AB->f3;
_tmp3B0=_tmp3AF.f1;_tmp3B1=_tmp3AB->f4;}}_LL225:{const char*_tmp8E9;const char*
_tmp8E8;const char*_tmp8E7;const char*_tmp8E6;struct Cyc_PP_Doc*_tmp8E5[2];struct
Cyc_PP_Doc*_tmp8E4[11];s=((_tmp8E4[10]=Cyc_Absynpp_rb(),((_tmp8E4[9]=Cyc_PP_line_doc(),((
_tmp8E4[8]=Cyc_PP_nest(2,((_tmp8E5[1]=Cyc_Absynpp_stmt2doc(_tmp3B1,0),((_tmp8E5[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8E5,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp8E4[7]=Cyc_Absynpp_lb(),((_tmp8E4[6]=Cyc_PP_text(((_tmp8E6=") ",
_tag_dyneither(_tmp8E6,sizeof(char),3)))),((_tmp8E4[5]=Cyc_Absynpp_exp2doc(
_tmp3B0),((_tmp8E4[4]=Cyc_PP_text(((_tmp8E7="; ",_tag_dyneither(_tmp8E7,sizeof(
char),3)))),((_tmp8E4[3]=Cyc_Absynpp_exp2doc(_tmp3AE),((_tmp8E4[2]=Cyc_PP_text(((
_tmp8E8="; ",_tag_dyneither(_tmp8E8,sizeof(char),3)))),((_tmp8E4[1]=Cyc_Absynpp_exp2doc(
_tmp3AC),((_tmp8E4[0]=Cyc_PP_text(((_tmp8E9="for(",_tag_dyneither(_tmp8E9,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8E4,sizeof(struct Cyc_PP_Doc*),
11))))))))))))))))))))))));}goto _LL211;_LL226: {struct Cyc_Absyn_Switch_s_struct*
_tmp3B2=(struct Cyc_Absyn_Switch_s_struct*)_tmp396;if(_tmp3B2->tag != 10)goto
_LL228;else{_tmp3B3=_tmp3B2->f1;_tmp3B4=_tmp3B2->f2;}}_LL227:{const char*_tmp8EE;
const char*_tmp8ED;struct Cyc_PP_Doc*_tmp8EC[8];s=((_tmp8EC[7]=Cyc_Absynpp_rb(),((
_tmp8EC[6]=Cyc_PP_line_doc(),((_tmp8EC[5]=Cyc_Absynpp_switchclauses2doc(_tmp3B4),((
_tmp8EC[4]=Cyc_PP_line_doc(),((_tmp8EC[3]=Cyc_Absynpp_lb(),((_tmp8EC[2]=Cyc_PP_text(((
_tmp8ED=") ",_tag_dyneither(_tmp8ED,sizeof(char),3)))),((_tmp8EC[1]=Cyc_Absynpp_exp2doc(
_tmp3B3),((_tmp8EC[0]=Cyc_PP_text(((_tmp8EE="switch (",_tag_dyneither(_tmp8EE,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp8EC,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL211;_LL228: {struct Cyc_Absyn_Fallthru_s_struct*_tmp3B5=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp396;if(_tmp3B5->tag != 11)goto _LL22A;else{_tmp3B6=_tmp3B5->f1;if(_tmp3B6 != 0)
goto _LL22A;}}_LL229:{const char*_tmp8EF;s=Cyc_PP_text(((_tmp8EF="fallthru;",
_tag_dyneither(_tmp8EF,sizeof(char),10))));}goto _LL211;_LL22A: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp3B7=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp396;if(_tmp3B7->tag != 11)goto
_LL22C;else{_tmp3B8=_tmp3B7->f1;}}_LL22B:{const char*_tmp8F4;const char*_tmp8F3;
struct Cyc_PP_Doc*_tmp8F2[3];s=((_tmp8F2[2]=Cyc_PP_text(((_tmp8F3=");",
_tag_dyneither(_tmp8F3,sizeof(char),3)))),((_tmp8F2[1]=Cyc_Absynpp_exps2doc_prec(
20,_tmp3B8),((_tmp8F2[0]=Cyc_PP_text(((_tmp8F4="fallthru(",_tag_dyneither(
_tmp8F4,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp8F2,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LL211;_LL22C: {struct Cyc_Absyn_Decl_s_struct*_tmp3B9=(struct Cyc_Absyn_Decl_s_struct*)
_tmp396;if(_tmp3B9->tag != 12)goto _LL22E;else{_tmp3BA=_tmp3B9->f1;_tmp3BB=_tmp3B9->f2;}}
_LL22D:{struct Cyc_PP_Doc*_tmp8F5[3];s=((_tmp8F5[2]=Cyc_Absynpp_stmt2doc(_tmp3BB,
stmtexp),((_tmp8F5[1]=Cyc_PP_line_doc(),((_tmp8F5[0]=Cyc_Absynpp_decl2doc(
_tmp3BA),Cyc_PP_cat(_tag_dyneither(_tmp8F5,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL211;_LL22E: {struct Cyc_Absyn_Label_s_struct*_tmp3BC=(struct Cyc_Absyn_Label_s_struct*)
_tmp396;if(_tmp3BC->tag != 13)goto _LL230;else{_tmp3BD=_tmp3BC->f1;_tmp3BE=_tmp3BC->f2;}}
_LL22F: if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp3BE)){if(
stmtexp){const char*_tmp8FA;const char*_tmp8F9;struct Cyc_PP_Doc*_tmp8F8[8];s=((
_tmp8F8[7]=Cyc_PP_text(((_tmp8F9=");",_tag_dyneither(_tmp8F9,sizeof(char),3)))),((
_tmp8F8[6]=Cyc_Absynpp_rb(),((_tmp8F8[5]=Cyc_PP_line_doc(),((_tmp8F8[4]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp3BE,1)),((_tmp8F8[3]=Cyc_PP_line_doc(),((_tmp8F8[2]=
Cyc_Absynpp_lb(),((_tmp8F8[1]=Cyc_PP_text(((_tmp8FA=": (",_tag_dyneither(_tmp8FA,
sizeof(char),4)))),((_tmp8F8[0]=Cyc_PP_textptr(_tmp3BD),Cyc_PP_cat(
_tag_dyneither(_tmp8F8,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{const
char*_tmp8FD;struct Cyc_PP_Doc*_tmp8FC[7];s=((_tmp8FC[6]=Cyc_Absynpp_rb(),((
_tmp8FC[5]=Cyc_PP_line_doc(),((_tmp8FC[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3BE,0)),((_tmp8FC[3]=Cyc_PP_line_doc(),((_tmp8FC[2]=Cyc_Absynpp_lb(),((
_tmp8FC[1]=Cyc_PP_text(((_tmp8FD=": ",_tag_dyneither(_tmp8FD,sizeof(char),3)))),((
_tmp8FC[0]=Cyc_PP_textptr(_tmp3BD),Cyc_PP_cat(_tag_dyneither(_tmp8FC,sizeof(
struct Cyc_PP_Doc*),7))))))))))))))));}}else{const char*_tmp900;struct Cyc_PP_Doc*
_tmp8FF[3];s=((_tmp8FF[2]=Cyc_Absynpp_stmt2doc(_tmp3BE,stmtexp),((_tmp8FF[1]=Cyc_PP_text(((
_tmp900=": ",_tag_dyneither(_tmp900,sizeof(char),3)))),((_tmp8FF[0]=Cyc_PP_textptr(
_tmp3BD),Cyc_PP_cat(_tag_dyneither(_tmp8FF,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL211;_LL230: {struct Cyc_Absyn_Do_s_struct*_tmp3BF=(struct Cyc_Absyn_Do_s_struct*)
_tmp396;if(_tmp3BF->tag != 14)goto _LL232;else{_tmp3C0=_tmp3BF->f1;_tmp3C1=_tmp3BF->f2;
_tmp3C2=_tmp3C1.f1;}}_LL231:{const char*_tmp907;const char*_tmp906;const char*
_tmp905;struct Cyc_PP_Doc*_tmp904[9];s=((_tmp904[8]=Cyc_PP_text(((_tmp905=");",
_tag_dyneither(_tmp905,sizeof(char),3)))),((_tmp904[7]=Cyc_Absynpp_exp2doc(
_tmp3C2),((_tmp904[6]=Cyc_PP_text(((_tmp906=" while (",_tag_dyneither(_tmp906,
sizeof(char),9)))),((_tmp904[5]=Cyc_Absynpp_rb(),((_tmp904[4]=Cyc_PP_line_doc(),((
_tmp904[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3C0,0)),((_tmp904[2]=Cyc_PP_line_doc(),((
_tmp904[1]=Cyc_Absynpp_lb(),((_tmp904[0]=Cyc_PP_text(((_tmp907="do ",
_tag_dyneither(_tmp907,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp904,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL211;_LL232: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp3C3=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp396;if(_tmp3C3->tag != 15)goto
_LL234;else{_tmp3C4=_tmp3C3->f1;_tmp3C5=_tmp3C3->f2;}}_LL233:{const char*_tmp90C;
const char*_tmp90B;struct Cyc_PP_Doc*_tmp90A[12];s=((_tmp90A[11]=Cyc_Absynpp_rb(),((
_tmp90A[10]=Cyc_PP_line_doc(),((_tmp90A[9]=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(
_tmp3C5)),((_tmp90A[8]=Cyc_PP_line_doc(),((_tmp90A[7]=Cyc_Absynpp_lb(),((_tmp90A[
6]=Cyc_PP_text(((_tmp90B=" catch ",_tag_dyneither(_tmp90B,sizeof(char),8)))),((
_tmp90A[5]=Cyc_Absynpp_rb(),((_tmp90A[4]=Cyc_PP_line_doc(),((_tmp90A[3]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp3C4,0)),((_tmp90A[2]=Cyc_PP_line_doc(),((_tmp90A[1]=
Cyc_Absynpp_lb(),((_tmp90A[0]=Cyc_PP_text(((_tmp90C="try ",_tag_dyneither(
_tmp90C,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp90A,sizeof(struct Cyc_PP_Doc*),
12))))))))))))))))))))))))));}goto _LL211;_LL234: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp3C6=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp396;if(_tmp3C6->tag != 16)goto
_LL211;else{_tmp3C7=_tmp3C6->f1;}}_LL235:{const char*_tmp911;const char*_tmp910;
struct Cyc_PP_Doc*_tmp90F[3];s=((_tmp90F[2]=Cyc_PP_text(((_tmp910=");",
_tag_dyneither(_tmp910,sizeof(char),3)))),((_tmp90F[1]=Cyc_Absynpp_exp2doc(
_tmp3C7),((_tmp90F[0]=Cyc_PP_text(((_tmp911="reset_region(",_tag_dyneither(
_tmp911,sizeof(char),14)))),Cyc_PP_cat(_tag_dyneither(_tmp90F,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LL211;_LL211:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat*p);struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*
p){struct Cyc_PP_Doc*s;{void*_tmp40C=p->r;enum Cyc_Absyn_Sign _tmp410;int _tmp411;
char _tmp413;struct _dyneither_ptr _tmp415;struct Cyc_Absyn_Vardecl*_tmp417;struct
Cyc_Absyn_Pat*_tmp418;struct Cyc_Absyn_Pat _tmp419;void*_tmp41A;struct Cyc_Absyn_Vardecl*
_tmp41D;struct Cyc_Absyn_Pat*_tmp41E;struct Cyc_Absyn_Tvar*_tmp420;struct Cyc_Absyn_Vardecl*
_tmp421;struct Cyc_List_List*_tmp423;int _tmp424;struct Cyc_Absyn_Pat*_tmp426;
struct Cyc_Absyn_Vardecl*_tmp428;struct Cyc_Absyn_Pat*_tmp429;struct Cyc_Absyn_Pat
_tmp42A;void*_tmp42B;struct Cyc_Absyn_Vardecl*_tmp42E;struct Cyc_Absyn_Pat*_tmp42F;
struct _tuple0*_tmp431;struct _tuple0*_tmp433;struct Cyc_List_List*_tmp434;int
_tmp435;struct Cyc_Absyn_AggrInfo*_tmp437;struct Cyc_Absyn_AggrInfo _tmp438;union
Cyc_Absyn_AggrInfoU _tmp439;struct Cyc_List_List*_tmp43A;struct Cyc_List_List*
_tmp43B;int _tmp43C;struct Cyc_Absyn_AggrInfo*_tmp43E;struct Cyc_List_List*_tmp43F;
struct Cyc_List_List*_tmp440;int _tmp441;struct Cyc_Absyn_Enumfield*_tmp443;struct
Cyc_Absyn_Enumfield*_tmp445;struct Cyc_Absyn_Datatypefield*_tmp447;struct Cyc_List_List*
_tmp448;struct Cyc_Absyn_Datatypefield*_tmp44A;struct Cyc_List_List*_tmp44B;int
_tmp44C;struct Cyc_Absyn_Exp*_tmp44E;_LL23C: {struct Cyc_Absyn_Wild_p_struct*
_tmp40D=(struct Cyc_Absyn_Wild_p_struct*)_tmp40C;if(_tmp40D->tag != 0)goto _LL23E;}
_LL23D:{const char*_tmp912;s=Cyc_PP_text(((_tmp912="_",_tag_dyneither(_tmp912,
sizeof(char),2))));}goto _LL23B;_LL23E: {struct Cyc_Absyn_Null_p_struct*_tmp40E=(
struct Cyc_Absyn_Null_p_struct*)_tmp40C;if(_tmp40E->tag != 8)goto _LL240;}_LL23F:{
const char*_tmp913;s=Cyc_PP_text(((_tmp913="NULL",_tag_dyneither(_tmp913,sizeof(
char),5))));}goto _LL23B;_LL240: {struct Cyc_Absyn_Int_p_struct*_tmp40F=(struct Cyc_Absyn_Int_p_struct*)
_tmp40C;if(_tmp40F->tag != 9)goto _LL242;else{_tmp410=_tmp40F->f1;_tmp411=_tmp40F->f2;}}
_LL241: if(_tmp410 != (enum Cyc_Absyn_Sign)Cyc_Absyn_Unsigned){const char*_tmp917;
void*_tmp916[1];struct Cyc_Int_pa_struct _tmp915;s=Cyc_PP_text((struct
_dyneither_ptr)((_tmp915.tag=1,((_tmp915.f1=(unsigned long)_tmp411,((_tmp916[0]=&
_tmp915,Cyc_aprintf(((_tmp917="%d",_tag_dyneither(_tmp917,sizeof(char),3))),
_tag_dyneither(_tmp916,sizeof(void*),1)))))))));}else{const char*_tmp91B;void*
_tmp91A[1];struct Cyc_Int_pa_struct _tmp919;s=Cyc_PP_text((struct _dyneither_ptr)((
_tmp919.tag=1,((_tmp919.f1=(unsigned int)_tmp411,((_tmp91A[0]=& _tmp919,Cyc_aprintf(((
_tmp91B="%u",_tag_dyneither(_tmp91B,sizeof(char),3))),_tag_dyneither(_tmp91A,
sizeof(void*),1)))))))));}goto _LL23B;_LL242: {struct Cyc_Absyn_Char_p_struct*
_tmp412=(struct Cyc_Absyn_Char_p_struct*)_tmp40C;if(_tmp412->tag != 10)goto _LL244;
else{_tmp413=_tmp412->f1;}}_LL243:{const char*_tmp91F;void*_tmp91E[1];struct Cyc_String_pa_struct
_tmp91D;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp91D.tag=0,((_tmp91D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp413)),((
_tmp91E[0]=& _tmp91D,Cyc_aprintf(((_tmp91F="'%s'",_tag_dyneither(_tmp91F,sizeof(
char),5))),_tag_dyneither(_tmp91E,sizeof(void*),1)))))))));}goto _LL23B;_LL244: {
struct Cyc_Absyn_Float_p_struct*_tmp414=(struct Cyc_Absyn_Float_p_struct*)_tmp40C;
if(_tmp414->tag != 11)goto _LL246;else{_tmp415=_tmp414->f1;}}_LL245: s=Cyc_PP_text(
_tmp415);goto _LL23B;_LL246: {struct Cyc_Absyn_Var_p_struct*_tmp416=(struct Cyc_Absyn_Var_p_struct*)
_tmp40C;if(_tmp416->tag != 1)goto _LL248;else{_tmp417=_tmp416->f1;_tmp418=_tmp416->f2;
_tmp419=*_tmp418;_tmp41A=_tmp419.r;{struct Cyc_Absyn_Wild_p_struct*_tmp41B=(
struct Cyc_Absyn_Wild_p_struct*)_tmp41A;if(_tmp41B->tag != 0)goto _LL248;};}}_LL247:
s=Cyc_Absynpp_qvar2doc(_tmp417->name);goto _LL23B;_LL248: {struct Cyc_Absyn_Var_p_struct*
_tmp41C=(struct Cyc_Absyn_Var_p_struct*)_tmp40C;if(_tmp41C->tag != 1)goto _LL24A;
else{_tmp41D=_tmp41C->f1;_tmp41E=_tmp41C->f2;}}_LL249:{const char*_tmp922;struct
Cyc_PP_Doc*_tmp921[3];s=((_tmp921[2]=Cyc_Absynpp_pat2doc(_tmp41E),((_tmp921[1]=
Cyc_PP_text(((_tmp922=" as ",_tag_dyneither(_tmp922,sizeof(char),5)))),((_tmp921[
0]=Cyc_Absynpp_qvar2doc(_tmp41D->name),Cyc_PP_cat(_tag_dyneither(_tmp921,sizeof(
struct Cyc_PP_Doc*),3))))))));}goto _LL23B;_LL24A: {struct Cyc_Absyn_TagInt_p_struct*
_tmp41F=(struct Cyc_Absyn_TagInt_p_struct*)_tmp40C;if(_tmp41F->tag != 3)goto _LL24C;
else{_tmp420=_tmp41F->f1;_tmp421=_tmp41F->f2;}}_LL24B:{const char*_tmp927;const
char*_tmp926;struct Cyc_PP_Doc*_tmp925[4];s=((_tmp925[3]=Cyc_PP_text(((_tmp926=">",
_tag_dyneither(_tmp926,sizeof(char),2)))),((_tmp925[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(
_tmp420)),((_tmp925[1]=Cyc_PP_text(((_tmp927="<",_tag_dyneither(_tmp927,sizeof(
char),2)))),((_tmp925[0]=Cyc_Absynpp_qvar2doc(_tmp421->name),Cyc_PP_cat(
_tag_dyneither(_tmp925,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL23B;_LL24C: {
struct Cyc_Absyn_Tuple_p_struct*_tmp422=(struct Cyc_Absyn_Tuple_p_struct*)_tmp40C;
if(_tmp422->tag != 4)goto _LL24E;else{_tmp423=_tmp422->f1;_tmp424=_tmp422->f2;}}
_LL24D:{const char*_tmp930;const char*_tmp92F;const char*_tmp92E;const char*_tmp92D;
struct Cyc_PP_Doc*_tmp92C[4];s=((_tmp92C[3]=_tmp424?Cyc_PP_text(((_tmp92D=", ...)",
_tag_dyneither(_tmp92D,sizeof(char),7)))): Cyc_PP_text(((_tmp92E=")",
_tag_dyneither(_tmp92E,sizeof(char),2)))),((_tmp92C[2]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,((_tmp92F=",",_tag_dyneither(_tmp92F,
sizeof(char),2))),_tmp423),((_tmp92C[1]=Cyc_PP_text(((_tmp930="(",_tag_dyneither(
_tmp930,sizeof(char),2)))),((_tmp92C[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(
_tag_dyneither(_tmp92C,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL23B;_LL24E: {
struct Cyc_Absyn_Pointer_p_struct*_tmp425=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp40C;if(_tmp425->tag != 5)goto _LL250;else{_tmp426=_tmp425->f1;}}_LL24F:{const
char*_tmp933;struct Cyc_PP_Doc*_tmp932[2];s=((_tmp932[1]=Cyc_Absynpp_pat2doc(
_tmp426),((_tmp932[0]=Cyc_PP_text(((_tmp933="&",_tag_dyneither(_tmp933,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp932,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL23B;_LL250: {struct Cyc_Absyn_Reference_p_struct*_tmp427=(struct Cyc_Absyn_Reference_p_struct*)
_tmp40C;if(_tmp427->tag != 2)goto _LL252;else{_tmp428=_tmp427->f1;_tmp429=_tmp427->f2;
_tmp42A=*_tmp429;_tmp42B=_tmp42A.r;{struct Cyc_Absyn_Wild_p_struct*_tmp42C=(
struct Cyc_Absyn_Wild_p_struct*)_tmp42B;if(_tmp42C->tag != 0)goto _LL252;};}}_LL251:{
const char*_tmp936;struct Cyc_PP_Doc*_tmp935[2];s=((_tmp935[1]=Cyc_Absynpp_qvar2doc(
_tmp428->name),((_tmp935[0]=Cyc_PP_text(((_tmp936="*",_tag_dyneither(_tmp936,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp935,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL23B;_LL252: {struct Cyc_Absyn_Reference_p_struct*_tmp42D=(struct Cyc_Absyn_Reference_p_struct*)
_tmp40C;if(_tmp42D->tag != 2)goto _LL254;else{_tmp42E=_tmp42D->f1;_tmp42F=_tmp42D->f2;}}
_LL253:{const char*_tmp93B;const char*_tmp93A;struct Cyc_PP_Doc*_tmp939[4];s=((
_tmp939[3]=Cyc_Absynpp_pat2doc(_tmp42F),((_tmp939[2]=Cyc_PP_text(((_tmp93A=" as ",
_tag_dyneither(_tmp93A,sizeof(char),5)))),((_tmp939[1]=Cyc_Absynpp_qvar2doc(
_tmp42E->name),((_tmp939[0]=Cyc_PP_text(((_tmp93B="*",_tag_dyneither(_tmp93B,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp939,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL23B;_LL254: {struct Cyc_Absyn_UnknownId_p_struct*_tmp430=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp40C;if(_tmp430->tag != 14)goto _LL256;else{_tmp431=_tmp430->f1;}}_LL255: s=Cyc_Absynpp_qvar2doc(
_tmp431);goto _LL23B;_LL256: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp432=(
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp40C;if(_tmp432->tag != 15)goto _LL258;
else{_tmp433=_tmp432->f1;_tmp434=_tmp432->f2;_tmp435=_tmp432->f3;}}_LL257: {
const char*_tmp93D;const char*_tmp93C;struct _dyneither_ptr term=_tmp435?(_tmp93D=", ...)",
_tag_dyneither(_tmp93D,sizeof(char),7)):((_tmp93C=")",_tag_dyneither(_tmp93C,
sizeof(char),2)));{const char*_tmp942;const char*_tmp941;struct Cyc_PP_Doc*_tmp940[
2];s=((_tmp940[1]=Cyc_PP_group(((_tmp942="(",_tag_dyneither(_tmp942,sizeof(char),
2))),term,((_tmp941=",",_tag_dyneither(_tmp941,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_pat2doc,_tmp434)),((_tmp940[0]=Cyc_Absynpp_qvar2doc(_tmp433),Cyc_PP_cat(
_tag_dyneither(_tmp940,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL23B;}_LL258: {
struct Cyc_Absyn_Aggr_p_struct*_tmp436=(struct Cyc_Absyn_Aggr_p_struct*)_tmp40C;
if(_tmp436->tag != 6)goto _LL25A;else{_tmp437=_tmp436->f1;if(_tmp437 == 0)goto
_LL25A;_tmp438=*_tmp437;_tmp439=_tmp438.aggr_info;_tmp43A=_tmp436->f2;_tmp43B=
_tmp436->f3;_tmp43C=_tmp436->f4;}}_LL259: {const char*_tmp944;const char*_tmp943;
struct _dyneither_ptr term=_tmp43C?(_tmp944=", ...}",_tag_dyneither(_tmp944,
sizeof(char),7)):((_tmp943="}",_tag_dyneither(_tmp943,sizeof(char),2)));struct
_tuple0*_tmp471;struct _tuple9 _tmp470=Cyc_Absyn_aggr_kinded_name(_tmp439);_tmp471=
_tmp470.f2;{const char*_tmp94F;const char*_tmp94E;const char*_tmp94D;const char*
_tmp94C;const char*_tmp94B;struct Cyc_PP_Doc*_tmp94A[4];s=((_tmp94A[3]=Cyc_PP_group(((
_tmp94C="",_tag_dyneither(_tmp94C,sizeof(char),1))),term,((_tmp94B=",",
_tag_dyneither(_tmp94B,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp43B)),((_tmp94A[2]=Cyc_PP_egroup(((_tmp94F="[",_tag_dyneither(_tmp94F,
sizeof(char),2))),((_tmp94E="]",_tag_dyneither(_tmp94E,sizeof(char),2))),((
_tmp94D=",",_tag_dyneither(_tmp94D,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp43A))),((_tmp94A[1]=
Cyc_Absynpp_lb(),((_tmp94A[0]=Cyc_Absynpp_qvar2doc(_tmp471),Cyc_PP_cat(
_tag_dyneither(_tmp94A,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL23B;}_LL25A: {
struct Cyc_Absyn_Aggr_p_struct*_tmp43D=(struct Cyc_Absyn_Aggr_p_struct*)_tmp40C;
if(_tmp43D->tag != 6)goto _LL25C;else{_tmp43E=_tmp43D->f1;if(_tmp43E != 0)goto
_LL25C;_tmp43F=_tmp43D->f2;_tmp440=_tmp43D->f3;_tmp441=_tmp43D->f4;}}_LL25B: {
const char*_tmp951;const char*_tmp950;struct _dyneither_ptr term=_tmp441?(_tmp951=", ...}",
_tag_dyneither(_tmp951,sizeof(char),7)):((_tmp950="}",_tag_dyneither(_tmp950,
sizeof(char),2)));{const char*_tmp95C;const char*_tmp95B;const char*_tmp95A;const
char*_tmp959;const char*_tmp958;struct Cyc_PP_Doc*_tmp957[3];s=((_tmp957[2]=Cyc_PP_group(((
_tmp959="",_tag_dyneither(_tmp959,sizeof(char),1))),term,((_tmp958=",",
_tag_dyneither(_tmp958,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp440)),((_tmp957[1]=Cyc_PP_egroup(((_tmp95C="[",_tag_dyneither(_tmp95C,
sizeof(char),2))),((_tmp95B="]",_tag_dyneither(_tmp95B,sizeof(char),2))),((
_tmp95A=",",_tag_dyneither(_tmp95A,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp43F))),((_tmp957[0]=
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp957,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL23B;}_LL25C: {struct Cyc_Absyn_Enum_p_struct*_tmp442=(struct Cyc_Absyn_Enum_p_struct*)
_tmp40C;if(_tmp442->tag != 12)goto _LL25E;else{_tmp443=_tmp442->f2;}}_LL25D:
_tmp445=_tmp443;goto _LL25F;_LL25E: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp444=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp40C;if(_tmp444->tag != 13)goto _LL260;else{
_tmp445=_tmp444->f2;}}_LL25F: s=Cyc_Absynpp_qvar2doc(_tmp445->name);goto _LL23B;
_LL260: {struct Cyc_Absyn_Datatype_p_struct*_tmp446=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp40C;if(_tmp446->tag != 7)goto _LL262;else{_tmp447=_tmp446->f2;_tmp448=_tmp446->f3;
if(_tmp448 != 0)goto _LL262;}}_LL261: s=Cyc_Absynpp_qvar2doc(_tmp447->name);goto
_LL23B;_LL262: {struct Cyc_Absyn_Datatype_p_struct*_tmp449=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp40C;if(_tmp449->tag != 7)goto _LL264;else{_tmp44A=_tmp449->f2;_tmp44B=_tmp449->f3;
_tmp44C=_tmp449->f4;}}_LL263: {const char*_tmp95E;const char*_tmp95D;struct
_dyneither_ptr term=_tmp44C?(_tmp95E=", ...)",_tag_dyneither(_tmp95E,sizeof(char),
7)):((_tmp95D=")",_tag_dyneither(_tmp95D,sizeof(char),2)));{const char*_tmp963;
const char*_tmp962;struct Cyc_PP_Doc*_tmp961[2];s=((_tmp961[1]=Cyc_PP_egroup(((
_tmp963="(",_tag_dyneither(_tmp963,sizeof(char),2))),term,((_tmp962=",",
_tag_dyneither(_tmp962,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,
_tmp44B)),((_tmp961[0]=Cyc_Absynpp_qvar2doc(_tmp44A->name),Cyc_PP_cat(
_tag_dyneither(_tmp961,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL23B;}_LL264: {
struct Cyc_Absyn_Exp_p_struct*_tmp44D=(struct Cyc_Absyn_Exp_p_struct*)_tmp40C;if(
_tmp44D->tag != 16)goto _LL23B;else{_tmp44E=_tmp44D->f1;}}_LL265: s=Cyc_Absynpp_exp2doc(
_tmp44E);goto _LL23B;_LL23B:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct
_tuple14*dp);struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple14*dp){const char*
_tmp96A;const char*_tmp969;const char*_tmp968;struct Cyc_PP_Doc*_tmp967[2];return(
_tmp967[1]=Cyc_Absynpp_pat2doc((*dp).f2),((_tmp967[0]=Cyc_PP_egroup(((_tmp96A="",
_tag_dyneither(_tmp96A,sizeof(char),1))),((_tmp969="=",_tag_dyneither(_tmp969,
sizeof(char),2))),((_tmp968="=",_tag_dyneither(_tmp968,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*dp).f1)),Cyc_PP_cat(_tag_dyneither(_tmp967,sizeof(
struct Cyc_PP_Doc*),2)))));}struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct
Cyc_Absyn_Switch_clause*c);struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct
Cyc_Absyn_Switch_clause*c){if(c->where_clause == 0  && (c->pattern)->r == (void*)&
Cyc_Absyn_Wild_p_val){const char*_tmp96F;struct Cyc_PP_Doc*_tmp96E[2];struct Cyc_PP_Doc*
_tmp96D[2];return(_tmp96D[1]=Cyc_PP_nest(2,((_tmp96E[1]=Cyc_Absynpp_stmt2doc(c->body,
0),((_tmp96E[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp96E,sizeof(
struct Cyc_PP_Doc*),2))))))),((_tmp96D[0]=Cyc_PP_text(((_tmp96F="default: ",
_tag_dyneither(_tmp96F,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp96D,
sizeof(struct Cyc_PP_Doc*),2)))));}else{if(c->where_clause == 0){const char*_tmp976;
const char*_tmp975;struct Cyc_PP_Doc*_tmp974[2];struct Cyc_PP_Doc*_tmp973[4];return(
_tmp973[3]=Cyc_PP_nest(2,((_tmp974[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp974[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp974,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp973[2]=Cyc_PP_text(((_tmp975=": ",_tag_dyneither(_tmp975,sizeof(
char),3)))),((_tmp973[1]=Cyc_Absynpp_pat2doc(c->pattern),((_tmp973[0]=Cyc_PP_text(((
_tmp976="case ",_tag_dyneither(_tmp976,sizeof(char),6)))),Cyc_PP_cat(
_tag_dyneither(_tmp973,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{const char*
_tmp97F;const char*_tmp97E;const char*_tmp97D;struct Cyc_PP_Doc*_tmp97C[2];struct
Cyc_PP_Doc*_tmp97B[6];return(_tmp97B[5]=Cyc_PP_nest(2,((_tmp97C[1]=Cyc_Absynpp_stmt2doc(
c->body,0),((_tmp97C[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp97C,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp97B[4]=Cyc_PP_text(((_tmp97D=": ",
_tag_dyneither(_tmp97D,sizeof(char),3)))),((_tmp97B[3]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(c->where_clause)),((_tmp97B[2]=Cyc_PP_text(((
_tmp97E=" && ",_tag_dyneither(_tmp97E,sizeof(char),5)))),((_tmp97B[1]=Cyc_Absynpp_pat2doc(
c->pattern),((_tmp97B[0]=Cyc_PP_text(((_tmp97F="case ",_tag_dyneither(_tmp97F,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp97B,sizeof(struct Cyc_PP_Doc*),6)))))))))))));}}}
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs);struct Cyc_PP_Doc*
Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){const char*_tmp980;return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,((
_tmp980="",_tag_dyneither(_tmp980,sizeof(char),1))),cs);}struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(
struct Cyc_Absyn_Enumfield*f);struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct
Cyc_Absyn_Enumfield*f){if(f->tag == 0)return Cyc_Absynpp_qvar2doc(f->name);else{
const char*_tmp983;struct Cyc_PP_Doc*_tmp982[3];return(_tmp982[2]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->tag)),((_tmp982[1]=Cyc_PP_text(((_tmp983=" = ",
_tag_dyneither(_tmp983,sizeof(char),4)))),((_tmp982[0]=Cyc_Absynpp_qvar2doc(f->name),
Cyc_PP_cat(_tag_dyneither(_tmp982,sizeof(struct Cyc_PP_Doc*),3)))))));}}struct Cyc_PP_Doc*
Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs);struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(
struct Cyc_List_List*fs){const char*_tmp984;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))
Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,((_tmp984=",",_tag_dyneither(_tmp984,
sizeof(char),2))),fs);}static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*
v);static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){return
Cyc_Absynpp_qvar2doc(v->name);}static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct
Cyc_List_List*vds);static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*
vds){const char*_tmp985;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(
Cyc_Absynpp_id2doc,((_tmp985=",",_tag_dyneither(_tmp985,sizeof(char),2))),vds);}
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd);struct Cyc_PP_Doc*
Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){struct Cyc_Absyn_Vardecl
_tmp49D;enum Cyc_Absyn_Scope _tmp49E;struct _tuple0*_tmp49F;struct Cyc_Absyn_Tqual
_tmp4A0;void*_tmp4A1;struct Cyc_Absyn_Exp*_tmp4A2;struct Cyc_List_List*_tmp4A3;
struct Cyc_Absyn_Vardecl*_tmp49C=vd;_tmp49D=*_tmp49C;_tmp49E=_tmp49D.sc;_tmp49F=
_tmp49D.name;_tmp4A0=_tmp49D.tq;_tmp4A1=_tmp49D.type;_tmp4A2=_tmp49D.initializer;
_tmp4A3=_tmp49D.attributes;{struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(
_tmp49F);struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4A3);struct Cyc_PP_Doc*
beforenamedoc;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL266:
beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL267:{void*_tmp4A4=Cyc_Tcutil_compress(
_tmp4A1);struct Cyc_Absyn_FnInfo _tmp4A6;struct Cyc_List_List*_tmp4A7;_LL26A: {
struct Cyc_Absyn_FnType_struct*_tmp4A5=(struct Cyc_Absyn_FnType_struct*)_tmp4A4;
if(_tmp4A5->tag != 10)goto _LL26C;else{_tmp4A6=_tmp4A5->f1;_tmp4A7=_tmp4A6.attributes;}}
_LL26B: beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp4A7);goto _LL269;_LL26C:;_LL26D:
beforenamedoc=Cyc_PP_nil_doc();goto _LL269;_LL269:;}break;}{struct Cyc_PP_Doc*
tmp_doc;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL26E: tmp_doc=Cyc_PP_nil_doc();
break;case Cyc_Cyclone_Vc_c: _LL26F: tmp_doc=attsdoc;break;}{struct Cyc_Core_Opt*
_tmp992;struct Cyc_PP_Doc*_tmp991[2];struct Cyc_PP_Doc*_tmp990[2];const char*
_tmp98F;const char*_tmp98E;struct Cyc_PP_Doc*_tmp98D[5];s=((_tmp98D[4]=Cyc_PP_text(((
_tmp98E=";",_tag_dyneither(_tmp98E,sizeof(char),2)))),((_tmp98D[3]=_tmp4A2 == 0?
Cyc_PP_nil_doc():((_tmp990[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp4A2),((
_tmp990[0]=Cyc_PP_text(((_tmp98F=" = ",_tag_dyneither(_tmp98F,sizeof(char),4)))),
Cyc_PP_cat(_tag_dyneither(_tmp990,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp98D[2]=
Cyc_Absynpp_tqtd2doc(_tmp4A0,_tmp4A1,((_tmp992=_cycalloc(sizeof(*_tmp992)),((
_tmp992->v=((_tmp991[1]=sn,((_tmp991[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(
_tmp991,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp992))))),((_tmp98D[1]=Cyc_Absynpp_scope2doc(
_tmp49E),((_tmp98D[0]=tmp_doc,Cyc_PP_cat(_tag_dyneither(_tmp98D,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}return s;};};}struct _tuple17{struct Cyc_Position_Segment*f1;struct
_tuple0*f2;int f3;};struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x);
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x){struct _tuple0*_tmp4AF;
struct _tuple17 _tmp4AE=*x;_tmp4AF=_tmp4AE.f2;return Cyc_Absynpp_qvar2doc(_tmp4AF);}
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d);struct Cyc_PP_Doc*
Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){struct Cyc_PP_Doc*s;{void*_tmp4B0=d->r;
struct Cyc_Absyn_Fndecl*_tmp4B2;struct Cyc_Absyn_Aggrdecl*_tmp4B4;struct Cyc_Absyn_Vardecl*
_tmp4B6;struct Cyc_Absyn_Tvar*_tmp4B8;struct Cyc_Absyn_Vardecl*_tmp4B9;int _tmp4BA;
struct Cyc_Absyn_Exp*_tmp4BB;struct Cyc_Absyn_Exp*_tmp4BD;struct Cyc_Absyn_Tvar*
_tmp4BE;struct Cyc_Absyn_Vardecl*_tmp4BF;struct Cyc_Absyn_Datatypedecl*_tmp4C1;
struct Cyc_Absyn_Datatypedecl _tmp4C2;enum Cyc_Absyn_Scope _tmp4C3;struct _tuple0*
_tmp4C4;struct Cyc_List_List*_tmp4C5;struct Cyc_Core_Opt*_tmp4C6;int _tmp4C7;struct
Cyc_Absyn_Pat*_tmp4C9;struct Cyc_Absyn_Exp*_tmp4CA;struct Cyc_List_List*_tmp4CC;
struct Cyc_Absyn_Enumdecl*_tmp4CE;struct Cyc_Absyn_Enumdecl _tmp4CF;enum Cyc_Absyn_Scope
_tmp4D0;struct _tuple0*_tmp4D1;struct Cyc_Core_Opt*_tmp4D2;struct Cyc_Absyn_Typedefdecl*
_tmp4D4;struct _dyneither_ptr*_tmp4D6;struct Cyc_List_List*_tmp4D7;struct _tuple0*
_tmp4D9;struct Cyc_List_List*_tmp4DA;struct Cyc_List_List*_tmp4DC;struct Cyc_List_List*
_tmp4DE;struct Cyc_List_List*_tmp4DF;_LL272: {struct Cyc_Absyn_Fn_d_struct*_tmp4B1=(
struct Cyc_Absyn_Fn_d_struct*)_tmp4B0;if(_tmp4B1->tag != 1)goto _LL274;else{_tmp4B2=
_tmp4B1->f1;}}_LL273: {struct Cyc_Absyn_FnType_struct _tmp998;struct Cyc_Absyn_FnInfo
_tmp997;struct Cyc_Absyn_FnType_struct*_tmp996;void*t=(void*)((_tmp996=_cycalloc(
sizeof(*_tmp996)),((_tmp996[0]=((_tmp998.tag=10,((_tmp998.f1=((_tmp997.tvars=
_tmp4B2->tvs,((_tmp997.effect=_tmp4B2->effect,((_tmp997.ret_typ=_tmp4B2->ret_type,((
_tmp997.args=((struct Cyc_List_List*(*)(struct _tuple7*(*f)(struct _tuple12*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp4B2->args),((
_tmp997.c_varargs=_tmp4B2->c_varargs,((_tmp997.cyc_varargs=_tmp4B2->cyc_varargs,((
_tmp997.rgn_po=_tmp4B2->rgn_po,((_tmp997.attributes=0,_tmp997)))))))))))))))),
_tmp998)))),_tmp996))));struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4B2->attributes);
struct Cyc_PP_Doc*inlinedoc;if(_tmp4B2->is_inline)switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LL292:{const char*_tmp999;inlinedoc=Cyc_PP_text(((_tmp999="inline ",
_tag_dyneither(_tmp999,sizeof(char),8))));}break;case Cyc_Cyclone_Vc_c: _LL293:{
const char*_tmp99A;inlinedoc=Cyc_PP_text(((_tmp99A="__inline ",_tag_dyneither(
_tmp99A,sizeof(char),10))));}break;}else{inlinedoc=Cyc_PP_nil_doc();}{struct Cyc_PP_Doc*
scopedoc=Cyc_Absynpp_scope2doc(_tmp4B2->sc);struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL295: beforenamedoc=
attsdoc;break;case Cyc_Cyclone_Vc_c: _LL296: beforenamedoc=Cyc_Absynpp_callconv2doc(
_tmp4B2->attributes);break;}{struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(
_tmp4B2->name);struct Cyc_PP_Doc*_tmp99D[2];struct Cyc_Core_Opt*_tmp99C;struct Cyc_PP_Doc*
tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,((_tmp99C=_cycalloc(
sizeof(*_tmp99C)),((_tmp99C->v=((_tmp99D[1]=namedoc,((_tmp99D[0]=beforenamedoc,
Cyc_PP_cat(_tag_dyneither(_tmp99D,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp99C)))));
struct Cyc_PP_Doc*_tmp9A0[2];struct Cyc_PP_Doc*_tmp99F[5];struct Cyc_PP_Doc*bodydoc=(
_tmp99F[4]=Cyc_Absynpp_rb(),((_tmp99F[3]=Cyc_PP_line_doc(),((_tmp99F[2]=Cyc_PP_nest(
2,((_tmp9A0[1]=Cyc_Absynpp_stmt2doc(_tmp4B2->body,0),((_tmp9A0[0]=Cyc_PP_line_doc(),
Cyc_PP_cat(_tag_dyneither(_tmp9A0,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp99F[1]=
Cyc_Absynpp_lb(),((_tmp99F[0]=Cyc_PP_blank_doc(),Cyc_PP_cat(_tag_dyneither(
_tmp99F,sizeof(struct Cyc_PP_Doc*),5)))))))))));{struct Cyc_PP_Doc*_tmp9A1[4];s=((
_tmp9A1[3]=bodydoc,((_tmp9A1[2]=tqtddoc,((_tmp9A1[1]=scopedoc,((_tmp9A1[0]=
inlinedoc,Cyc_PP_cat(_tag_dyneither(_tmp9A1,sizeof(struct Cyc_PP_Doc*),4))))))))));}
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL298:{struct Cyc_PP_Doc*
_tmp9A2[2];s=((_tmp9A2[1]=s,((_tmp9A2[0]=attsdoc,Cyc_PP_cat(_tag_dyneither(
_tmp9A2,sizeof(struct Cyc_PP_Doc*),2))))));}break;default: _LL299: break;}goto
_LL271;};};}_LL274: {struct Cyc_Absyn_Aggr_d_struct*_tmp4B3=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp4B0;if(_tmp4B3->tag != 6)goto _LL276;else{_tmp4B4=_tmp4B3->f1;}}_LL275: if(
_tmp4B4->impl == 0){const char*_tmp9A5;struct Cyc_PP_Doc*_tmp9A4[5];s=((_tmp9A4[4]=
Cyc_PP_text(((_tmp9A5=";",_tag_dyneither(_tmp9A5,sizeof(char),2)))),((_tmp9A4[3]=
Cyc_Absynpp_ktvars2doc(_tmp4B4->tvs),((_tmp9A4[2]=Cyc_Absynpp_qvar2bolddoc(
_tmp4B4->name),((_tmp9A4[1]=Cyc_Absynpp_aggr_kind2doc(_tmp4B4->kind),((_tmp9A4[0]=
Cyc_Absynpp_scope2doc(_tmp4B4->sc),Cyc_PP_cat(_tag_dyneither(_tmp9A4,sizeof(
struct Cyc_PP_Doc*),5))))))))))));}else{struct Cyc_PP_Doc*_tmp9AF[2];const char*
_tmp9AE;struct Cyc_PP_Doc*_tmp9AD[2];const char*_tmp9AC;struct Cyc_PP_Doc*_tmp9AB[
13];s=((_tmp9AB[12]=Cyc_PP_text(((_tmp9AC=";",_tag_dyneither(_tmp9AC,sizeof(char),
2)))),((_tmp9AB[11]=Cyc_Absynpp_atts2doc(_tmp4B4->attributes),((_tmp9AB[10]=Cyc_Absynpp_rb(),((
_tmp9AB[9]=Cyc_PP_line_doc(),((_tmp9AB[8]=Cyc_PP_nest(2,((_tmp9AD[1]=Cyc_Absynpp_aggrfields2doc(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4B4->impl))->fields),((_tmp9AD[0]=
Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9AD,sizeof(struct Cyc_PP_Doc*),2))))))),((
_tmp9AB[7]=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4B4->impl))->rgn_po == 
0?Cyc_PP_nil_doc():((_tmp9AF[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4B4->impl))->rgn_po),((_tmp9AF[0]=Cyc_PP_text(((_tmp9AE=":",
_tag_dyneither(_tmp9AE,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp9AF,
sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp9AB[6]=Cyc_Absynpp_ktvars2doc(((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4B4->impl))->exist_vars),((_tmp9AB[5]=
Cyc_Absynpp_lb(),((_tmp9AB[4]=Cyc_PP_blank_doc(),((_tmp9AB[3]=Cyc_Absynpp_ktvars2doc(
_tmp4B4->tvs),((_tmp9AB[2]=Cyc_Absynpp_qvar2bolddoc(_tmp4B4->name),((_tmp9AB[1]=
Cyc_Absynpp_aggr_kind2doc(_tmp4B4->kind),((_tmp9AB[0]=Cyc_Absynpp_scope2doc(
_tmp4B4->sc),Cyc_PP_cat(_tag_dyneither(_tmp9AB,sizeof(struct Cyc_PP_Doc*),13))))))))))))))))))))))))))));}
goto _LL271;_LL276: {struct Cyc_Absyn_Var_d_struct*_tmp4B5=(struct Cyc_Absyn_Var_d_struct*)
_tmp4B0;if(_tmp4B5->tag != 0)goto _LL278;else{_tmp4B6=_tmp4B5->f1;}}_LL277: s=Cyc_Absynpp_vardecl2doc(
_tmp4B6);goto _LL271;_LL278: {struct Cyc_Absyn_Region_d_struct*_tmp4B7=(struct Cyc_Absyn_Region_d_struct*)
_tmp4B0;if(_tmp4B7->tag != 4)goto _LL27A;else{_tmp4B8=_tmp4B7->f1;_tmp4B9=_tmp4B7->f2;
_tmp4BA=_tmp4B7->f3;_tmp4BB=_tmp4B7->f4;}}_LL279:{const char*_tmp9C2;const char*
_tmp9C1;const char*_tmp9C0;const char*_tmp9BF;struct Cyc_PP_Doc*_tmp9BE[3];const
char*_tmp9BD;const char*_tmp9BC;const char*_tmp9BB;struct Cyc_PP_Doc*_tmp9BA[8];s=((
_tmp9BA[7]=Cyc_PP_text(((_tmp9BB=";",_tag_dyneither(_tmp9BB,sizeof(char),2)))),((
_tmp9BA[6]=_tmp4BB != 0?(_tmp9BE[2]=Cyc_PP_text(((_tmp9BD=")",_tag_dyneither(
_tmp9BD,sizeof(char),2)))),((_tmp9BE[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_tmp4BB),((_tmp9BE[0]=Cyc_PP_text(((_tmp9BC=" = open(",_tag_dyneither(_tmp9BC,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp9BE,sizeof(struct Cyc_PP_Doc*),3))))))):
Cyc_PP_nil_doc(),((_tmp9BA[5]=_tmp4BA?Cyc_PP_text(((_tmp9BF=" @resetable",
_tag_dyneither(_tmp9BF,sizeof(char),12)))): Cyc_PP_nil_doc(),((_tmp9BA[4]=Cyc_Absynpp_qvar2doc(
_tmp4B9->name),((_tmp9BA[3]=Cyc_PP_text(((_tmp9C0=">",_tag_dyneither(_tmp9C0,
sizeof(char),2)))),((_tmp9BA[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp4B8)),((
_tmp9BA[1]=Cyc_PP_text(((_tmp9C1="<",_tag_dyneither(_tmp9C1,sizeof(char),2)))),((
_tmp9BA[0]=Cyc_PP_text(((_tmp9C2="region",_tag_dyneither(_tmp9C2,sizeof(char),7)))),
Cyc_PP_cat(_tag_dyneither(_tmp9BA,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL271;_LL27A: {struct Cyc_Absyn_Alias_d_struct*_tmp4BC=(struct Cyc_Absyn_Alias_d_struct*)
_tmp4B0;if(_tmp4BC->tag != 5)goto _LL27C;else{_tmp4BD=_tmp4BC->f1;_tmp4BE=_tmp4BC->f2;
_tmp4BF=_tmp4BC->f3;}}_LL27B:{const char*_tmp9CD;const char*_tmp9CC;const char*
_tmp9CB;const char*_tmp9CA;const char*_tmp9C9;struct Cyc_PP_Doc*_tmp9C8[8];s=((
_tmp9C8[7]=Cyc_PP_text(((_tmp9C9=";",_tag_dyneither(_tmp9C9,sizeof(char),2)))),((
_tmp9C8[6]=Cyc_Absynpp_exp2doc(_tmp4BD),((_tmp9C8[5]=Cyc_PP_text(((_tmp9CA=" = ",
_tag_dyneither(_tmp9CA,sizeof(char),4)))),((_tmp9C8[4]=Cyc_Absynpp_qvar2doc(
_tmp4BF->name),((_tmp9C8[3]=Cyc_PP_text(((_tmp9CB="> ",_tag_dyneither(_tmp9CB,
sizeof(char),3)))),((_tmp9C8[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp4BE)),((
_tmp9C8[1]=Cyc_PP_text(((_tmp9CC=" <",_tag_dyneither(_tmp9CC,sizeof(char),3)))),((
_tmp9C8[0]=Cyc_PP_text(((_tmp9CD="alias",_tag_dyneither(_tmp9CD,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmp9C8,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL271;_LL27C: {struct Cyc_Absyn_Datatype_d_struct*_tmp4C0=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp4B0;if(_tmp4C0->tag != 7)goto _LL27E;else{_tmp4C1=_tmp4C0->f1;_tmp4C2=*_tmp4C1;
_tmp4C3=_tmp4C2.sc;_tmp4C4=_tmp4C2.name;_tmp4C5=_tmp4C2.tvs;_tmp4C6=_tmp4C2.fields;
_tmp4C7=_tmp4C2.is_extensible;}}_LL27D: if(_tmp4C6 == 0){const char*_tmp9D4;const
char*_tmp9D3;const char*_tmp9D2;struct Cyc_PP_Doc*_tmp9D1[6];s=((_tmp9D1[5]=Cyc_PP_text(((
_tmp9D2=";",_tag_dyneither(_tmp9D2,sizeof(char),2)))),((_tmp9D1[4]=Cyc_Absynpp_ktvars2doc(
_tmp4C5),((_tmp9D1[3]=_tmp4C7?Cyc_Absynpp_qvar2bolddoc(_tmp4C4): Cyc_Absynpp_typedef_name2bolddoc(
_tmp4C4),((_tmp9D1[2]=Cyc_PP_text(((_tmp9D3="datatype ",_tag_dyneither(_tmp9D3,
sizeof(char),10)))),((_tmp9D1[1]=_tmp4C7?Cyc_PP_text(((_tmp9D4="@extensible",
_tag_dyneither(_tmp9D4,sizeof(char),12)))): Cyc_PP_blank_doc(),((_tmp9D1[0]=Cyc_Absynpp_scope2doc(
_tmp4C3),Cyc_PP_cat(_tag_dyneither(_tmp9D1,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
else{const char*_tmp9DD;const char*_tmp9DC;struct Cyc_PP_Doc*_tmp9DB[2];const char*
_tmp9DA;struct Cyc_PP_Doc*_tmp9D9[11];s=((_tmp9D9[10]=Cyc_PP_text(((_tmp9DA=";",
_tag_dyneither(_tmp9DA,sizeof(char),2)))),((_tmp9D9[9]=Cyc_Absynpp_rb(),((
_tmp9D9[8]=Cyc_PP_line_doc(),((_tmp9D9[7]=Cyc_PP_nest(2,((_tmp9DB[1]=Cyc_Absynpp_datatypefields2doc((
struct Cyc_List_List*)_tmp4C6->v),((_tmp9DB[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp9DB,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9D9[6]=Cyc_Absynpp_lb(),((
_tmp9D9[5]=Cyc_PP_blank_doc(),((_tmp9D9[4]=Cyc_Absynpp_ktvars2doc(_tmp4C5),((
_tmp9D9[3]=_tmp4C7?Cyc_Absynpp_qvar2bolddoc(_tmp4C4): Cyc_Absynpp_typedef_name2bolddoc(
_tmp4C4),((_tmp9D9[2]=Cyc_PP_text(((_tmp9DC="datatype ",_tag_dyneither(_tmp9DC,
sizeof(char),10)))),((_tmp9D9[1]=_tmp4C7?Cyc_PP_text(((_tmp9DD="@extensible ",
_tag_dyneither(_tmp9DD,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp9D9[0]=Cyc_Absynpp_scope2doc(
_tmp4C3),Cyc_PP_cat(_tag_dyneither(_tmp9D9,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL271;_LL27E: {struct Cyc_Absyn_Let_d_struct*_tmp4C8=(struct Cyc_Absyn_Let_d_struct*)
_tmp4B0;if(_tmp4C8->tag != 2)goto _LL280;else{_tmp4C9=_tmp4C8->f1;_tmp4CA=_tmp4C8->f3;}}
_LL27F:{const char*_tmp9E4;const char*_tmp9E3;const char*_tmp9E2;struct Cyc_PP_Doc*
_tmp9E1[5];s=((_tmp9E1[4]=Cyc_PP_text(((_tmp9E2=";",_tag_dyneither(_tmp9E2,
sizeof(char),2)))),((_tmp9E1[3]=Cyc_Absynpp_exp2doc(_tmp4CA),((_tmp9E1[2]=Cyc_PP_text(((
_tmp9E3=" = ",_tag_dyneither(_tmp9E3,sizeof(char),4)))),((_tmp9E1[1]=Cyc_Absynpp_pat2doc(
_tmp4C9),((_tmp9E1[0]=Cyc_PP_text(((_tmp9E4="let ",_tag_dyneither(_tmp9E4,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp9E1,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL271;_LL280: {struct Cyc_Absyn_Letv_d_struct*_tmp4CB=(struct Cyc_Absyn_Letv_d_struct*)
_tmp4B0;if(_tmp4CB->tag != 3)goto _LL282;else{_tmp4CC=_tmp4CB->f1;}}_LL281:{const
char*_tmp9E9;const char*_tmp9E8;struct Cyc_PP_Doc*_tmp9E7[3];s=((_tmp9E7[2]=Cyc_PP_text(((
_tmp9E8=";",_tag_dyneither(_tmp9E8,sizeof(char),2)))),((_tmp9E7[1]=Cyc_Absynpp_ids2doc(
_tmp4CC),((_tmp9E7[0]=Cyc_PP_text(((_tmp9E9="let ",_tag_dyneither(_tmp9E9,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp9E7,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL271;_LL282: {struct Cyc_Absyn_Enum_d_struct*_tmp4CD=(struct Cyc_Absyn_Enum_d_struct*)
_tmp4B0;if(_tmp4CD->tag != 8)goto _LL284;else{_tmp4CE=_tmp4CD->f1;_tmp4CF=*_tmp4CE;
_tmp4D0=_tmp4CF.sc;_tmp4D1=_tmp4CF.name;_tmp4D2=_tmp4CF.fields;}}_LL283: if(
_tmp4D2 == 0){const char*_tmp9EE;const char*_tmp9ED;struct Cyc_PP_Doc*_tmp9EC[4];s=((
_tmp9EC[3]=Cyc_PP_text(((_tmp9ED=";",_tag_dyneither(_tmp9ED,sizeof(char),2)))),((
_tmp9EC[2]=Cyc_Absynpp_typedef_name2bolddoc(_tmp4D1),((_tmp9EC[1]=Cyc_PP_text(((
_tmp9EE="enum ",_tag_dyneither(_tmp9EE,sizeof(char),6)))),((_tmp9EC[0]=Cyc_Absynpp_scope2doc(
_tmp4D0),Cyc_PP_cat(_tag_dyneither(_tmp9EC,sizeof(struct Cyc_PP_Doc*),4))))))))));}
else{const char*_tmp9F5;struct Cyc_PP_Doc*_tmp9F4[2];const char*_tmp9F3;struct Cyc_PP_Doc*
_tmp9F2[9];s=((_tmp9F2[8]=Cyc_PP_text(((_tmp9F3=";",_tag_dyneither(_tmp9F3,
sizeof(char),2)))),((_tmp9F2[7]=Cyc_Absynpp_rb(),((_tmp9F2[6]=Cyc_PP_line_doc(),((
_tmp9F2[5]=Cyc_PP_nest(2,((_tmp9F4[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)
_tmp4D2->v),((_tmp9F4[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9F4,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9F2[4]=Cyc_Absynpp_lb(),((_tmp9F2[3]=
Cyc_PP_blank_doc(),((_tmp9F2[2]=Cyc_Absynpp_qvar2bolddoc(_tmp4D1),((_tmp9F2[1]=
Cyc_PP_text(((_tmp9F5="enum ",_tag_dyneither(_tmp9F5,sizeof(char),6)))),((
_tmp9F2[0]=Cyc_Absynpp_scope2doc(_tmp4D0),Cyc_PP_cat(_tag_dyneither(_tmp9F2,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL271;_LL284: {struct Cyc_Absyn_Typedef_d_struct*
_tmp4D3=(struct Cyc_Absyn_Typedef_d_struct*)_tmp4B0;if(_tmp4D3->tag != 9)goto
_LL286;else{_tmp4D4=_tmp4D3->f1;}}_LL285: {void*t;if(_tmp4D4->defn != 0)t=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4D4->defn))->v;else{t=Cyc_Absyn_new_evar(
_tmp4D4->kind,0);}{const char*_tmp9FF;struct Cyc_Core_Opt*_tmp9FE;struct Cyc_PP_Doc*
_tmp9FD[2];const char*_tmp9FC;struct Cyc_PP_Doc*_tmp9FB[4];s=((_tmp9FB[3]=Cyc_PP_text(((
_tmp9FC=";",_tag_dyneither(_tmp9FC,sizeof(char),2)))),((_tmp9FB[2]=Cyc_Absynpp_atts2doc(
_tmp4D4->atts),((_tmp9FB[1]=Cyc_Absynpp_tqtd2doc(_tmp4D4->tq,t,((_tmp9FE=
_cycalloc(sizeof(*_tmp9FE)),((_tmp9FE->v=((_tmp9FD[1]=Cyc_Absynpp_tvars2doc(
_tmp4D4->tvs),((_tmp9FD[0]=Cyc_Absynpp_typedef_name2bolddoc(_tmp4D4->name),Cyc_PP_cat(
_tag_dyneither(_tmp9FD,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9FE))))),((_tmp9FB[
0]=Cyc_PP_text(((_tmp9FF="typedef ",_tag_dyneither(_tmp9FF,sizeof(char),9)))),
Cyc_PP_cat(_tag_dyneither(_tmp9FB,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto
_LL271;}_LL286: {struct Cyc_Absyn_Namespace_d_struct*_tmp4D5=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp4B0;if(_tmp4D5->tag != 10)goto _LL288;else{_tmp4D6=_tmp4D5->f1;_tmp4D7=_tmp4D5->f2;}}
_LL287: if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp4D6);{
const char*_tmpA04;const char*_tmpA03;struct Cyc_PP_Doc*_tmpA02[8];s=((_tmpA02[7]=
Cyc_Absynpp_rb(),((_tmpA02[6]=Cyc_PP_line_doc(),((_tmpA02[5]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA03="",_tag_dyneither(_tmpA03,
sizeof(char),1))),_tmp4D7),((_tmpA02[4]=Cyc_PP_line_doc(),((_tmpA02[3]=Cyc_Absynpp_lb(),((
_tmpA02[2]=Cyc_PP_blank_doc(),((_tmpA02[1]=Cyc_PP_textptr(_tmp4D6),((_tmpA02[0]=
Cyc_PP_text(((_tmpA04="namespace ",_tag_dyneither(_tmpA04,sizeof(char),11)))),
Cyc_PP_cat(_tag_dyneither(_tmpA02,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();goto _LL271;
_LL288: {struct Cyc_Absyn_Using_d_struct*_tmp4D8=(struct Cyc_Absyn_Using_d_struct*)
_tmp4B0;if(_tmp4D8->tag != 11)goto _LL28A;else{_tmp4D9=_tmp4D8->f1;_tmp4DA=_tmp4D8->f2;}}
_LL289: if(Cyc_Absynpp_print_using_stmts){const char*_tmpA09;const char*_tmpA08;
struct Cyc_PP_Doc*_tmpA07[8];s=((_tmpA07[7]=Cyc_Absynpp_rb(),((_tmpA07[6]=Cyc_PP_line_doc(),((
_tmpA07[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA08="",_tag_dyneither(_tmpA08,sizeof(char),1))),_tmp4DA),((_tmpA07[4]=Cyc_PP_line_doc(),((
_tmpA07[3]=Cyc_Absynpp_lb(),((_tmpA07[2]=Cyc_PP_blank_doc(),((_tmpA07[1]=Cyc_Absynpp_qvar2doc(
_tmp4D9),((_tmpA07[0]=Cyc_PP_text(((_tmpA09="using ",_tag_dyneither(_tmpA09,
sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmpA07,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
else{const char*_tmpA14;const char*_tmpA13;const char*_tmpA12;const char*_tmpA11;
const char*_tmpA10;struct Cyc_PP_Doc*_tmpA0F[11];s=((_tmpA0F[10]=Cyc_PP_text(((
_tmpA10=" */",_tag_dyneither(_tmpA10,sizeof(char),4)))),((_tmpA0F[9]=Cyc_Absynpp_rb(),((
_tmpA0F[8]=Cyc_PP_text(((_tmpA11="/* ",_tag_dyneither(_tmpA11,sizeof(char),4)))),((
_tmpA0F[7]=Cyc_PP_line_doc(),((_tmpA0F[6]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmpA12="",_tag_dyneither(_tmpA12,sizeof(char),1))),
_tmp4DA),((_tmpA0F[5]=Cyc_PP_line_doc(),((_tmpA0F[4]=Cyc_PP_text(((_tmpA13=" */",
_tag_dyneither(_tmpA13,sizeof(char),4)))),((_tmpA0F[3]=Cyc_Absynpp_lb(),((
_tmpA0F[2]=Cyc_PP_blank_doc(),((_tmpA0F[1]=Cyc_Absynpp_qvar2doc(_tmp4D9),((
_tmpA0F[0]=Cyc_PP_text(((_tmpA14="/* using ",_tag_dyneither(_tmpA14,sizeof(char),
10)))),Cyc_PP_cat(_tag_dyneither(_tmpA0F,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL271;_LL28A: {struct Cyc_Absyn_ExternC_d_struct*_tmp4DB=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp4B0;if(_tmp4DB->tag != 12)goto _LL28C;else{_tmp4DC=_tmp4DB->f1;}}_LL28B: if(Cyc_Absynpp_print_externC_stmts){
const char*_tmpA19;const char*_tmpA18;struct Cyc_PP_Doc*_tmpA17[6];s=((_tmpA17[5]=
Cyc_Absynpp_rb(),((_tmpA17[4]=Cyc_PP_line_doc(),((_tmpA17[3]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA18="",_tag_dyneither(_tmpA18,
sizeof(char),1))),_tmp4DC),((_tmpA17[2]=Cyc_PP_line_doc(),((_tmpA17[1]=Cyc_Absynpp_lb(),((
_tmpA17[0]=Cyc_PP_text(((_tmpA19="extern \"C\" ",_tag_dyneither(_tmpA19,sizeof(
char),12)))),Cyc_PP_cat(_tag_dyneither(_tmpA17,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
else{const char*_tmpA24;const char*_tmpA23;const char*_tmpA22;const char*_tmpA21;
const char*_tmpA20;struct Cyc_PP_Doc*_tmpA1F[9];s=((_tmpA1F[8]=Cyc_PP_text(((
_tmpA20=" */",_tag_dyneither(_tmpA20,sizeof(char),4)))),((_tmpA1F[7]=Cyc_Absynpp_rb(),((
_tmpA1F[6]=Cyc_PP_text(((_tmpA21="/* ",_tag_dyneither(_tmpA21,sizeof(char),4)))),((
_tmpA1F[5]=Cyc_PP_line_doc(),((_tmpA1F[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmpA22="",_tag_dyneither(_tmpA22,sizeof(char),1))),
_tmp4DC),((_tmpA1F[3]=Cyc_PP_line_doc(),((_tmpA1F[2]=Cyc_PP_text(((_tmpA23=" */",
_tag_dyneither(_tmpA23,sizeof(char),4)))),((_tmpA1F[1]=Cyc_Absynpp_lb(),((
_tmpA1F[0]=Cyc_PP_text(((_tmpA24="/* extern \"C\" ",_tag_dyneither(_tmpA24,
sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmpA1F,sizeof(struct Cyc_PP_Doc*),
9))))))))))))))))))));}goto _LL271;_LL28C: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp4DD=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp4B0;if(_tmp4DD->tag != 13)
goto _LL28E;else{_tmp4DE=_tmp4DD->f1;_tmp4DF=_tmp4DD->f2;}}_LL28D: if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;if(_tmp4DF != 0){const char*_tmpA29;const char*_tmpA28;
struct Cyc_PP_Doc*_tmpA27[7];exs_doc=((_tmpA27[6]=Cyc_Absynpp_rb(),((_tmpA27[5]=
Cyc_PP_line_doc(),((_tmpA27[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_export2doc,((_tmpA28=",",_tag_dyneither(_tmpA28,sizeof(char),2))),
_tmp4DF),((_tmpA27[3]=Cyc_PP_line_doc(),((_tmpA27[2]=Cyc_Absynpp_lb(),((_tmpA27[
1]=Cyc_PP_text(((_tmpA29=" export ",_tag_dyneither(_tmpA29,sizeof(char),9)))),((
_tmpA27[0]=Cyc_Absynpp_rb(),Cyc_PP_cat(_tag_dyneither(_tmpA27,sizeof(struct Cyc_PP_Doc*),
7))))))))))))))));}else{exs_doc=Cyc_Absynpp_rb();}{const char*_tmpA2E;const char*
_tmpA2D;struct Cyc_PP_Doc*_tmpA2C[6];s=((_tmpA2C[5]=exs_doc,((_tmpA2C[4]=Cyc_PP_line_doc(),((
_tmpA2C[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA2D="",_tag_dyneither(_tmpA2D,sizeof(char),1))),_tmp4DE),((_tmpA2C[2]=Cyc_PP_line_doc(),((
_tmpA2C[1]=Cyc_Absynpp_lb(),((_tmpA2C[0]=Cyc_PP_text(((_tmpA2E="extern \"C include\" ",
_tag_dyneither(_tmpA2E,sizeof(char),20)))),Cyc_PP_cat(_tag_dyneither(_tmpA2C,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));};}else{const char*_tmpA39;const char*
_tmpA38;const char*_tmpA37;const char*_tmpA36;const char*_tmpA35;struct Cyc_PP_Doc*
_tmpA34[9];s=((_tmpA34[8]=Cyc_PP_text(((_tmpA35=" */",_tag_dyneither(_tmpA35,
sizeof(char),4)))),((_tmpA34[7]=Cyc_Absynpp_rb(),((_tmpA34[6]=Cyc_PP_text(((
_tmpA36="/* ",_tag_dyneither(_tmpA36,sizeof(char),4)))),((_tmpA34[5]=Cyc_PP_line_doc(),((
_tmpA34[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA37="",_tag_dyneither(_tmpA37,sizeof(char),1))),_tmp4DE),((_tmpA34[3]=Cyc_PP_line_doc(),((
_tmpA34[2]=Cyc_PP_text(((_tmpA38=" */",_tag_dyneither(_tmpA38,sizeof(char),4)))),((
_tmpA34[1]=Cyc_Absynpp_lb(),((_tmpA34[0]=Cyc_PP_text(((_tmpA39="/* extern \"C include\" ",
_tag_dyneither(_tmpA39,sizeof(char),23)))),Cyc_PP_cat(_tag_dyneither(_tmpA34,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL271;_LL28E: {struct Cyc_Absyn_Porton_d_struct*
_tmp4E0=(struct Cyc_Absyn_Porton_d_struct*)_tmp4B0;if(_tmp4E0->tag != 14)goto
_LL290;}_LL28F:{const char*_tmpA3C;struct Cyc_PP_Doc*_tmpA3B[2];s=((_tmpA3B[1]=Cyc_Absynpp_lb(),((
_tmpA3B[0]=Cyc_PP_text(((_tmpA3C="__cyclone_port_on__;",_tag_dyneither(_tmpA3C,
sizeof(char),21)))),Cyc_PP_cat(_tag_dyneither(_tmpA3B,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LL271;_LL290: {struct Cyc_Absyn_Portoff_d_struct*_tmp4E1=(struct Cyc_Absyn_Portoff_d_struct*)
_tmp4B0;if(_tmp4E1->tag != 15)goto _LL271;}_LL291:{const char*_tmpA3F;struct Cyc_PP_Doc*
_tmpA3E[2];s=((_tmpA3E[1]=Cyc_Absynpp_lb(),((_tmpA3E[0]=Cyc_PP_text(((_tmpA3F="__cyclone_port_off__;",
_tag_dyneither(_tmpA3F,sizeof(char),22)))),Cyc_PP_cat(_tag_dyneither(_tmpA3E,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL271;_LL271:;}return s;}struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc);struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(
enum Cyc_Absyn_Scope sc){if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();
switch(sc){case Cyc_Absyn_Static: _LL29B: {const char*_tmpA40;return Cyc_PP_text(((
_tmpA40="static ",_tag_dyneither(_tmpA40,sizeof(char),8))));}case Cyc_Absyn_Public:
_LL29C: return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL29D: {const char*_tmpA41;
return Cyc_PP_text(((_tmpA41="extern ",_tag_dyneither(_tmpA41,sizeof(char),8))));}
case Cyc_Absyn_ExternC: _LL29E: {const char*_tmpA42;return Cyc_PP_text(((_tmpA42="extern \"C\" ",
_tag_dyneither(_tmpA42,sizeof(char),12))));}case Cyc_Absyn_Abstract: _LL29F: {
const char*_tmpA43;return Cyc_PP_text(((_tmpA43="abstract ",_tag_dyneither(_tmpA43,
sizeof(char),10))));}case Cyc_Absyn_Register: _LL2A0: {const char*_tmpA44;return Cyc_PP_text(((
_tmpA44="register ",_tag_dyneither(_tmpA44,sizeof(char),10))));}}}int Cyc_Absynpp_exists_temp_tvar_in_effect(
void*t);int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){void*_tmp549=t;struct
Cyc_Absyn_Tvar*_tmp54B;struct Cyc_List_List*_tmp54D;_LL2A3: {struct Cyc_Absyn_VarType_struct*
_tmp54A=(struct Cyc_Absyn_VarType_struct*)_tmp549;if(_tmp54A->tag != 2)goto _LL2A5;
else{_tmp54B=_tmp54A->f1;}}_LL2A4: return Cyc_Tcutil_is_temp_tvar(_tmp54B);_LL2A5: {
struct Cyc_Absyn_JoinEff_struct*_tmp54C=(struct Cyc_Absyn_JoinEff_struct*)_tmp549;
if(_tmp54C->tag != 24)goto _LL2A7;else{_tmp54D=_tmp54C->f1;}}_LL2A6: return((int(*)(
int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,
_tmp54D);_LL2A7:;_LL2A8: return 0;_LL2A2:;}int Cyc_Absynpp_is_anon_aggrtype(void*t);
int Cyc_Absynpp_is_anon_aggrtype(void*t){void*_tmp54E=t;void**_tmp552;void*
_tmp553;_LL2AA: {struct Cyc_Absyn_AnonAggrType_struct*_tmp54F=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp54E;if(_tmp54F->tag != 13)goto _LL2AC;}_LL2AB: return 1;_LL2AC: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp550=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp54E;if(_tmp550->tag != 15)goto
_LL2AE;}_LL2AD: return 1;_LL2AE: {struct Cyc_Absyn_TypedefType_struct*_tmp551=(
struct Cyc_Absyn_TypedefType_struct*)_tmp54E;if(_tmp551->tag != 18)goto _LL2B0;
else{_tmp552=_tmp551->f4;if(_tmp552 == 0)goto _LL2B0;_tmp553=*_tmp552;}}_LL2AF:
return Cyc_Absynpp_is_anon_aggrtype(_tmp553);_LL2B0:;_LL2B1: return 0;_LL2A9:;}
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,
void*atts,struct Cyc_List_List*tms);static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(
struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){if(tms != 0  && tms->tl != 
0){struct _tuple11 _tmpA45;struct _tuple11 _tmp555=(_tmpA45.f1=(void*)tms->hd,((
_tmpA45.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,_tmpA45)));
void*_tmp556;void*_tmp558;_LL2B3: _tmp556=_tmp555.f1;{struct Cyc_Absyn_Pointer_mod_struct*
_tmp557=(struct Cyc_Absyn_Pointer_mod_struct*)_tmp556;if(_tmp557->tag != 2)goto
_LL2B5;};_tmp558=_tmp555.f2;{struct Cyc_Absyn_Function_mod_struct*_tmp559=(struct
Cyc_Absyn_Function_mod_struct*)_tmp558;if(_tmp559->tag != 3)goto _LL2B5;};_LL2B4: {
struct Cyc_List_List*_tmpA48;struct Cyc_List_List*_tmpA47;return(_tmpA47=
_region_malloc(r,sizeof(*_tmpA47)),((_tmpA47->hd=(void*)tms->hd,((_tmpA47->tl=((
_tmpA48=_region_malloc(r,sizeof(*_tmpA48)),((_tmpA48->hd=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd,((_tmpA48->tl=Cyc_Absynpp_bubble_attributes(r,atts,((
struct Cyc_List_List*)_check_null(tms->tl))->tl),_tmpA48)))))),_tmpA47)))));}
_LL2B5:;_LL2B6: {struct Cyc_List_List*_tmpA49;return(_tmpA49=_region_malloc(r,
sizeof(*_tmpA49)),((_tmpA49->hd=atts,((_tmpA49->tl=tms,_tmpA49)))));}_LL2B2:;}
else{struct Cyc_List_List*_tmpA4A;return(_tmpA4A=_region_malloc(r,sizeof(*_tmpA4A)),((
_tmpA4A->hd=atts,((_tmpA4A->tl=tms,_tmpA4A)))));}}struct _tuple13 Cyc_Absynpp_to_tms(
struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t);struct _tuple13 Cyc_Absynpp_to_tms(
struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){void*_tmp55E=t;struct Cyc_Absyn_ArrayInfo
_tmp560;void*_tmp561;struct Cyc_Absyn_Tqual _tmp562;struct Cyc_Absyn_Exp*_tmp563;
union Cyc_Absyn_Constraint*_tmp564;struct Cyc_Position_Segment*_tmp565;struct Cyc_Absyn_PtrInfo
_tmp567;void*_tmp568;struct Cyc_Absyn_Tqual _tmp569;struct Cyc_Absyn_PtrAtts _tmp56A;
struct Cyc_Absyn_FnInfo _tmp56C;struct Cyc_List_List*_tmp56D;struct Cyc_Core_Opt*
_tmp56E;void*_tmp56F;struct Cyc_List_List*_tmp570;int _tmp571;struct Cyc_Absyn_VarargInfo*
_tmp572;struct Cyc_List_List*_tmp573;struct Cyc_List_List*_tmp574;struct Cyc_Core_Opt*
_tmp576;struct Cyc_Core_Opt*_tmp577;int _tmp578;struct _tuple0*_tmp57A;struct Cyc_List_List*
_tmp57B;void**_tmp57C;_LL2B8: {struct Cyc_Absyn_ArrayType_struct*_tmp55F=(struct
Cyc_Absyn_ArrayType_struct*)_tmp55E;if(_tmp55F->tag != 9)goto _LL2BA;else{_tmp560=
_tmp55F->f1;_tmp561=_tmp560.elt_type;_tmp562=_tmp560.tq;_tmp563=_tmp560.num_elts;
_tmp564=_tmp560.zero_term;_tmp565=_tmp560.zt_loc;}}_LL2B9: {struct Cyc_Absyn_Tqual
_tmp57E;void*_tmp57F;struct Cyc_List_List*_tmp580;struct _tuple13 _tmp57D=Cyc_Absynpp_to_tms(
r,_tmp562,_tmp561);_tmp57E=_tmp57D.f1;_tmp57F=_tmp57D.f2;_tmp580=_tmp57D.f3;{
void*tm;if(_tmp563 == 0){struct Cyc_Absyn_Carray_mod_struct _tmpA4D;struct Cyc_Absyn_Carray_mod_struct*
_tmpA4C;tm=(void*)((_tmpA4C=_region_malloc(r,sizeof(*_tmpA4C)),((_tmpA4C[0]=((
_tmpA4D.tag=0,((_tmpA4D.f1=_tmp564,((_tmpA4D.f2=_tmp565,_tmpA4D)))))),_tmpA4C))));}
else{struct Cyc_Absyn_ConstArray_mod_struct _tmpA50;struct Cyc_Absyn_ConstArray_mod_struct*
_tmpA4F;tm=(void*)((_tmpA4F=_region_malloc(r,sizeof(*_tmpA4F)),((_tmpA4F[0]=((
_tmpA50.tag=1,((_tmpA50.f1=(struct Cyc_Absyn_Exp*)_tmp563,((_tmpA50.f2=_tmp564,((
_tmpA50.f3=_tmp565,_tmpA50)))))))),_tmpA4F))));}{struct Cyc_List_List*_tmpA53;
struct _tuple13 _tmpA52;return(_tmpA52.f1=_tmp57E,((_tmpA52.f2=_tmp57F,((_tmpA52.f3=((
_tmpA53=_region_malloc(r,sizeof(*_tmpA53)),((_tmpA53->hd=tm,((_tmpA53->tl=
_tmp580,_tmpA53)))))),_tmpA52)))));};};}_LL2BA: {struct Cyc_Absyn_PointerType_struct*
_tmp566=(struct Cyc_Absyn_PointerType_struct*)_tmp55E;if(_tmp566->tag != 5)goto
_LL2BC;else{_tmp567=_tmp566->f1;_tmp568=_tmp567.elt_typ;_tmp569=_tmp567.elt_tq;
_tmp56A=_tmp567.ptr_atts;}}_LL2BB: {struct Cyc_Absyn_Tqual _tmp588;void*_tmp589;
struct Cyc_List_List*_tmp58A;struct _tuple13 _tmp587=Cyc_Absynpp_to_tms(r,_tmp569,
_tmp568);_tmp588=_tmp587.f1;_tmp589=_tmp587.f2;_tmp58A=_tmp587.f3;{struct Cyc_Absyn_Pointer_mod_struct*
_tmpA59;struct Cyc_Absyn_Pointer_mod_struct _tmpA58;struct Cyc_List_List*_tmpA57;
_tmp58A=((_tmpA57=_region_malloc(r,sizeof(*_tmpA57)),((_tmpA57->hd=(void*)((
_tmpA59=_region_malloc(r,sizeof(*_tmpA59)),((_tmpA59[0]=((_tmpA58.tag=2,((
_tmpA58.f1=_tmp56A,((_tmpA58.f2=tq,_tmpA58)))))),_tmpA59)))),((_tmpA57->tl=
_tmp58A,_tmpA57))))));}{struct _tuple13 _tmpA5A;return(_tmpA5A.f1=_tmp588,((
_tmpA5A.f2=_tmp589,((_tmpA5A.f3=_tmp58A,_tmpA5A)))));};}_LL2BC: {struct Cyc_Absyn_FnType_struct*
_tmp56B=(struct Cyc_Absyn_FnType_struct*)_tmp55E;if(_tmp56B->tag != 10)goto _LL2BE;
else{_tmp56C=_tmp56B->f1;_tmp56D=_tmp56C.tvars;_tmp56E=_tmp56C.effect;_tmp56F=
_tmp56C.ret_typ;_tmp570=_tmp56C.args;_tmp571=_tmp56C.c_varargs;_tmp572=_tmp56C.cyc_varargs;
_tmp573=_tmp56C.rgn_po;_tmp574=_tmp56C.attributes;}}_LL2BD: if(!Cyc_Absynpp_print_all_tvars){
if(_tmp56E == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect((void*)_tmp56E->v)){
_tmp56E=0;_tmp56D=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,
_tmp56D);}{struct Cyc_Absyn_Tqual _tmp590;void*_tmp591;struct Cyc_List_List*_tmp592;
struct _tuple13 _tmp58F=Cyc_Absynpp_to_tms(r,Cyc_Absyn_empty_tqual(0),_tmp56F);
_tmp590=_tmp58F.f1;_tmp591=_tmp58F.f2;_tmp592=_tmp58F.f3;{struct Cyc_List_List*
tms=_tmp592;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2C4: if(
_tmp574 != 0){struct Cyc_Absyn_Attributes_mod_struct _tmpA5D;struct Cyc_Absyn_Attributes_mod_struct*
_tmpA5C;tms=Cyc_Absynpp_bubble_attributes(r,(void*)((_tmpA5C=_region_malloc(r,
sizeof(*_tmpA5C)),((_tmpA5C[0]=((_tmpA5D.tag=5,((_tmpA5D.f1=0,((_tmpA5D.f2=
_tmp574,_tmpA5D)))))),_tmpA5C)))),tms);}{struct Cyc_Absyn_Function_mod_struct*
_tmpA6C;struct Cyc_Absyn_WithTypes_struct*_tmpA6B;struct Cyc_Absyn_WithTypes_struct
_tmpA6A;struct Cyc_Absyn_Function_mod_struct _tmpA69;struct Cyc_List_List*_tmpA68;
tms=((_tmpA68=_region_malloc(r,sizeof(*_tmpA68)),((_tmpA68->hd=(void*)((_tmpA6C=
_region_malloc(r,sizeof(*_tmpA6C)),((_tmpA6C[0]=((_tmpA69.tag=3,((_tmpA69.f1=(
void*)((void*)((_tmpA6B=_region_malloc(r,sizeof(*_tmpA6B)),((_tmpA6B[0]=((
_tmpA6A.tag=1,((_tmpA6A.f1=_tmp570,((_tmpA6A.f2=_tmp571,((_tmpA6A.f3=_tmp572,((
_tmpA6A.f4=_tmp56E,((_tmpA6A.f5=_tmp573,_tmpA6A)))))))))))),_tmpA6B))))),_tmpA69)))),
_tmpA6C)))),((_tmpA68->tl=tms,_tmpA68))))));}break;case Cyc_Cyclone_Vc_c: _LL2C5:{
struct Cyc_Absyn_Function_mod_struct*_tmpA7B;struct Cyc_Absyn_WithTypes_struct*
_tmpA7A;struct Cyc_Absyn_WithTypes_struct _tmpA79;struct Cyc_Absyn_Function_mod_struct
_tmpA78;struct Cyc_List_List*_tmpA77;tms=((_tmpA77=_region_malloc(r,sizeof(*
_tmpA77)),((_tmpA77->hd=(void*)((_tmpA7B=_region_malloc(r,sizeof(*_tmpA7B)),((
_tmpA7B[0]=((_tmpA78.tag=3,((_tmpA78.f1=(void*)((void*)((_tmpA7A=_region_malloc(
r,sizeof(*_tmpA7A)),((_tmpA7A[0]=((_tmpA79.tag=1,((_tmpA79.f1=_tmp570,((_tmpA79.f2=
_tmp571,((_tmpA79.f3=_tmp572,((_tmpA79.f4=_tmp56E,((_tmpA79.f5=_tmp573,_tmpA79)))))))))))),
_tmpA7A))))),_tmpA78)))),_tmpA7B)))),((_tmpA77->tl=tms,_tmpA77))))));}for(0;
_tmp574 != 0;_tmp574=_tmp574->tl){void*_tmp59F=(void*)_tmp574->hd;_LL2C8: {struct
Cyc_Absyn_Stdcall_att_struct*_tmp5A0=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmp59F;if(_tmp5A0->tag != 1)goto _LL2CA;}_LL2C9: goto _LL2CB;_LL2CA: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp5A1=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp59F;if(_tmp5A1->tag != 2)goto
_LL2CC;}_LL2CB: goto _LL2CD;_LL2CC: {struct Cyc_Absyn_Fastcall_att_struct*_tmp5A2=(
struct Cyc_Absyn_Fastcall_att_struct*)_tmp59F;if(_tmp5A2->tag != 3)goto _LL2CE;}
_LL2CD:{struct Cyc_Absyn_Attributes_mod_struct*_tmpA85;struct Cyc_List_List*
_tmpA84;struct Cyc_Absyn_Attributes_mod_struct _tmpA83;struct Cyc_List_List*_tmpA82;
tms=((_tmpA82=_region_malloc(r,sizeof(*_tmpA82)),((_tmpA82->hd=(void*)((_tmpA85=
_region_malloc(r,sizeof(*_tmpA85)),((_tmpA85[0]=((_tmpA83.tag=5,((_tmpA83.f1=0,((
_tmpA83.f2=((_tmpA84=_cycalloc(sizeof(*_tmpA84)),((_tmpA84->hd=(void*)_tmp574->hd,((
_tmpA84->tl=0,_tmpA84)))))),_tmpA83)))))),_tmpA85)))),((_tmpA82->tl=tms,_tmpA82))))));}
goto AfterAtts;_LL2CE:;_LL2CF: goto _LL2C7;_LL2C7:;}break;}AfterAtts: if(_tmp56D != 0){
struct Cyc_Absyn_TypeParams_mod_struct*_tmpA8B;struct Cyc_Absyn_TypeParams_mod_struct
_tmpA8A;struct Cyc_List_List*_tmpA89;tms=((_tmpA89=_region_malloc(r,sizeof(*
_tmpA89)),((_tmpA89->hd=(void*)((_tmpA8B=_region_malloc(r,sizeof(*_tmpA8B)),((
_tmpA8B[0]=((_tmpA8A.tag=4,((_tmpA8A.f1=_tmp56D,((_tmpA8A.f2=0,((_tmpA8A.f3=1,
_tmpA8A)))))))),_tmpA8B)))),((_tmpA89->tl=tms,_tmpA89))))));}{struct _tuple13
_tmpA8C;return(_tmpA8C.f1=_tmp590,((_tmpA8C.f2=_tmp591,((_tmpA8C.f3=tms,_tmpA8C)))));};};};
_LL2BE: {struct Cyc_Absyn_Evar_struct*_tmp575=(struct Cyc_Absyn_Evar_struct*)
_tmp55E;if(_tmp575->tag != 1)goto _LL2C0;else{_tmp576=_tmp575->f1;_tmp577=_tmp575->f2;
_tmp578=_tmp575->f3;}}_LL2BF: if(_tmp577 == 0){struct _tuple13 _tmpA8D;return(
_tmpA8D.f1=tq,((_tmpA8D.f2=t,((_tmpA8D.f3=0,_tmpA8D)))));}else{return Cyc_Absynpp_to_tms(
r,tq,(void*)_tmp577->v);}_LL2C0: {struct Cyc_Absyn_TypedefType_struct*_tmp579=(
struct Cyc_Absyn_TypedefType_struct*)_tmp55E;if(_tmp579->tag != 18)goto _LL2C2;
else{_tmp57A=_tmp579->f1;_tmp57B=_tmp579->f2;_tmp57C=_tmp579->f4;}}_LL2C1: if((
_tmp57C == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype(*
_tmp57C)){struct _tuple13 _tmpA8E;return(_tmpA8E.f1=tq,((_tmpA8E.f2=t,((_tmpA8E.f3=
0,_tmpA8E)))));}else{return Cyc_Absynpp_to_tms(r,tq,*_tmp57C);}_LL2C2:;_LL2C3: {
struct _tuple13 _tmpA8F;return(_tmpA8F.f1=tq,((_tmpA8F.f2=t,((_tmpA8F.f3=0,_tmpA8F)))));}
_LL2B7:;}static int Cyc_Absynpp_is_char_ptr(void*t);static int Cyc_Absynpp_is_char_ptr(
void*t){void*_tmp5AE=t;struct Cyc_Core_Opt*_tmp5B0;struct Cyc_Core_Opt _tmp5B1;void*
_tmp5B2;struct Cyc_Absyn_PtrInfo _tmp5B4;void*_tmp5B5;_LL2D1: {struct Cyc_Absyn_Evar_struct*
_tmp5AF=(struct Cyc_Absyn_Evar_struct*)_tmp5AE;if(_tmp5AF->tag != 1)goto _LL2D3;
else{_tmp5B0=_tmp5AF->f2;if(_tmp5B0 == 0)goto _LL2D3;_tmp5B1=*_tmp5B0;_tmp5B2=(
void*)_tmp5B1.v;}}_LL2D2: return Cyc_Absynpp_is_char_ptr(_tmp5B2);_LL2D3: {struct
Cyc_Absyn_PointerType_struct*_tmp5B3=(struct Cyc_Absyn_PointerType_struct*)
_tmp5AE;if(_tmp5B3->tag != 5)goto _LL2D5;else{_tmp5B4=_tmp5B3->f1;_tmp5B5=_tmp5B4.elt_typ;}}
_LL2D4: L: {void*_tmp5B6=_tmp5B5;struct Cyc_Core_Opt*_tmp5B8;struct Cyc_Core_Opt
_tmp5B9;void*_tmp5BA;void**_tmp5BC;void*_tmp5BD;enum Cyc_Absyn_Size_of _tmp5BF;
_LL2D8: {struct Cyc_Absyn_Evar_struct*_tmp5B7=(struct Cyc_Absyn_Evar_struct*)
_tmp5B6;if(_tmp5B7->tag != 1)goto _LL2DA;else{_tmp5B8=_tmp5B7->f2;if(_tmp5B8 == 0)
goto _LL2DA;_tmp5B9=*_tmp5B8;_tmp5BA=(void*)_tmp5B9.v;}}_LL2D9: _tmp5B5=_tmp5BA;
goto L;_LL2DA: {struct Cyc_Absyn_TypedefType_struct*_tmp5BB=(struct Cyc_Absyn_TypedefType_struct*)
_tmp5B6;if(_tmp5BB->tag != 18)goto _LL2DC;else{_tmp5BC=_tmp5BB->f4;if(_tmp5BC == 0)
goto _LL2DC;_tmp5BD=*_tmp5BC;}}_LL2DB: _tmp5B5=_tmp5BD;goto L;_LL2DC: {struct Cyc_Absyn_IntType_struct*
_tmp5BE=(struct Cyc_Absyn_IntType_struct*)_tmp5B6;if(_tmp5BE->tag != 6)goto _LL2DE;
else{_tmp5BF=_tmp5BE->f2;if(_tmp5BF != Cyc_Absyn_Char_sz)goto _LL2DE;}}_LL2DD:
return 1;_LL2DE:;_LL2DF: return 0;_LL2D7:;}_LL2D5:;_LL2D6: return 0;_LL2D0:;}struct
Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*
dopt);struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,
struct Cyc_Core_Opt*dopt){struct _RegionHandle _tmp5C0=_new_region("temp");struct
_RegionHandle*temp=& _tmp5C0;_push_region(temp);{struct Cyc_Absyn_Tqual _tmp5C2;
void*_tmp5C3;struct Cyc_List_List*_tmp5C4;struct _tuple13 _tmp5C1=Cyc_Absynpp_to_tms(
temp,tq,typ);_tmp5C2=_tmp5C1.f1;_tmp5C3=_tmp5C1.f2;_tmp5C4=_tmp5C1.f3;_tmp5C4=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5C4);if(
_tmp5C4 == 0  && dopt == 0){struct Cyc_PP_Doc*_tmpA90[2];struct Cyc_PP_Doc*_tmp5C6=(
_tmpA90[1]=Cyc_Absynpp_ntyp2doc(_tmp5C3),((_tmpA90[0]=Cyc_Absynpp_tqual2doc(
_tmp5C2),Cyc_PP_cat(_tag_dyneither(_tmpA90,sizeof(struct Cyc_PP_Doc*),2)))));
_npop_handler(0);return _tmp5C6;}else{const char*_tmpA93;struct Cyc_PP_Doc*_tmpA92[
4];struct Cyc_PP_Doc*_tmp5C9=(_tmpA92[3]=Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(
typ),dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp5C4),((_tmpA92[2]=
Cyc_PP_text(((_tmpA93=" ",_tag_dyneither(_tmpA93,sizeof(char),2)))),((_tmpA92[1]=
Cyc_Absynpp_ntyp2doc(_tmp5C3),((_tmpA92[0]=Cyc_Absynpp_tqual2doc(_tmp5C2),Cyc_PP_cat(
_tag_dyneither(_tmpA92,sizeof(struct Cyc_PP_Doc*),4)))))))));_npop_handler(0);
return _tmp5C9;}};_pop_region(temp);}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(
struct Cyc_Absyn_Aggrfield*f);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct
Cyc_Absyn_Aggrfield*f){switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c:
_LL2E0: if(f->width != 0){struct Cyc_Core_Opt*_tmpA9A;const char*_tmpA99;const char*
_tmpA98;struct Cyc_PP_Doc*_tmpA97[5];return(_tmpA97[4]=Cyc_PP_text(((_tmpA98=";",
_tag_dyneither(_tmpA98,sizeof(char),2)))),((_tmpA97[3]=Cyc_Absynpp_atts2doc(f->attributes),((
_tmpA97[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((
_tmpA97[1]=Cyc_PP_text(((_tmpA99=":",_tag_dyneither(_tmpA99,sizeof(char),2)))),((
_tmpA97[0]=Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpA9A=_cycalloc(sizeof(*
_tmpA9A)),((_tmpA9A->v=Cyc_PP_textptr(f->name),_tmpA9A))))),Cyc_PP_cat(
_tag_dyneither(_tmpA97,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{struct Cyc_Core_Opt*
_tmpA9F;const char*_tmpA9E;struct Cyc_PP_Doc*_tmpA9D[3];return(_tmpA9D[2]=Cyc_PP_text(((
_tmpA9E=";",_tag_dyneither(_tmpA9E,sizeof(char),2)))),((_tmpA9D[1]=Cyc_Absynpp_atts2doc(
f->attributes),((_tmpA9D[0]=Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpA9F=
_cycalloc(sizeof(*_tmpA9F)),((_tmpA9F->v=Cyc_PP_textptr(f->name),_tmpA9F))))),
Cyc_PP_cat(_tag_dyneither(_tmpA9D,sizeof(struct Cyc_PP_Doc*),3)))))));}case Cyc_Cyclone_Vc_c:
_LL2E1: if(f->width != 0){struct Cyc_Core_Opt*_tmpAA6;const char*_tmpAA5;const char*
_tmpAA4;struct Cyc_PP_Doc*_tmpAA3[5];return(_tmpAA3[4]=Cyc_PP_text(((_tmpAA4=";",
_tag_dyneither(_tmpAA4,sizeof(char),2)))),((_tmpAA3[3]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpAA3[2]=Cyc_PP_text(((_tmpAA5=":",
_tag_dyneither(_tmpAA5,sizeof(char),2)))),((_tmpAA3[1]=Cyc_Absynpp_tqtd2doc(f->tq,
f->type,((_tmpAA6=_cycalloc(sizeof(*_tmpAA6)),((_tmpAA6->v=Cyc_PP_textptr(f->name),
_tmpAA6))))),((_tmpAA3[0]=Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(
_tag_dyneither(_tmpAA3,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{struct Cyc_Core_Opt*
_tmpAAB;const char*_tmpAAA;struct Cyc_PP_Doc*_tmpAA9[3];return(_tmpAA9[2]=Cyc_PP_text(((
_tmpAAA=";",_tag_dyneither(_tmpAAA,sizeof(char),2)))),((_tmpAA9[1]=Cyc_Absynpp_tqtd2doc(
f->tq,f->type,((_tmpAAB=_cycalloc(sizeof(*_tmpAAB)),((_tmpAAB->v=Cyc_PP_textptr(
f->name),_tmpAAB))))),((_tmpAA9[0]=Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(
_tag_dyneither(_tmpAA9,sizeof(struct Cyc_PP_Doc*),3)))))));}}}struct Cyc_PP_Doc*
Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(
struct Cyc_List_List*fields){const char*_tmpAAC;return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,
struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,((_tmpAAC="",
_tag_dyneither(_tmpAAC,sizeof(char),1))),fields);}struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(
struct Cyc_Absyn_Datatypefield*f);struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(
struct Cyc_Absyn_Datatypefield*f){struct Cyc_PP_Doc*_tmpAAD[3];return(_tmpAAD[2]=f->typs
== 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs),((_tmpAAD[1]=Cyc_Absynpp_typedef_name2doc(
f->name),((_tmpAAD[0]=Cyc_Absynpp_scope2doc(f->sc),Cyc_PP_cat(_tag_dyneither(
_tmpAAD,sizeof(struct Cyc_PP_Doc*),3)))))));}struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(
struct Cyc_List_List*fields){const char*_tmpAAE;return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _dyneither_ptr sep,
struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,((_tmpAAE=",",
_tag_dyneither(_tmpAAE,sizeof(char),2))),fields);}void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){for(0;tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(
Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);{const char*_tmpAB1;
void*_tmpAB0;(_tmpAB0=0,Cyc_fprintf(f,((_tmpAB1="\n",_tag_dyneither(_tmpAB1,
sizeof(char),2))),_tag_dyneither(_tmpAB0,sizeof(void*),0)));};}}struct
_dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);struct
_dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){const char*
_tmpAB2;return Cyc_PP_string_of_doc(Cyc_PP_seql(((_tmpAB2="",_tag_dyneither(
_tmpAB2,sizeof(char),1))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl)),
72);}struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(
Cyc_Absynpp_exp2doc(e),72);}struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct
Cyc_Absyn_Stmt*s);struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*
s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s,0),72);}struct
_dyneither_ptr Cyc_Absynpp_typ2string(void*t);struct _dyneither_ptr Cyc_Absynpp_typ2string(
void*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}struct
_dyneither_ptr Cyc_Absynpp_typ2cstring(void*t);struct _dyneither_ptr Cyc_Absynpp_typ2cstring(
void*t){int old_qvar_to_Cids=Cyc_Absynpp_qvar_to_Cids;int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;
Cyc_Absynpp_qvar_to_Cids=1;Cyc_Absynpp_add_cyc_prefix=0;{struct _dyneither_ptr s=
Cyc_Absynpp_typ2string(t);Cyc_Absynpp_qvar_to_Cids=old_qvar_to_Cids;Cyc_Absynpp_add_cyc_prefix=
old_add_cyc_prefix;return s;};}struct _dyneither_ptr Cyc_Absynpp_prim2string(enum 
Cyc_Absyn_Primop p);struct _dyneither_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop
p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}struct _dyneither_ptr
Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p);struct _dyneither_ptr Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc);struct
_dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(
Cyc_Absynpp_scope2doc(sc),72);}
