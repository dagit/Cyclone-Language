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
f1;};struct Cyc_Absyn_Offsetof_e_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_struct{
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
p);int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);struct _tuple11{struct
_dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct _tuple7*Cyc_Absynpp_arg_mk_opt(
struct _tuple11*arg);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*
f3;};struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual
tq,void*t);enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c
 = 1};extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;struct _tuple13{
struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(
struct _tuple13*dp);struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*
cs);struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope);
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*,int expstmt);struct
Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e);struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int
inprec,struct Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct
_tuple0*);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*);struct
Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst);struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(
enum Cyc_Absyn_Primop);struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum 
Cyc_Absyn_Primop p,struct Cyc_List_List*es);struct _tuple14{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de);
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
fs){Cyc_Absynpp_expand_typedefs=fs->expand_typedefs;Cyc_Absynpp_qvar_to_Cids=fs->qvar_to_Cids;
Cyc_Absynpp_add_cyc_prefix=fs->add_cyc_prefix;Cyc_Absynpp_to_VC=fs->to_VC;Cyc_Absynpp_decls_first=
fs->decls_first;Cyc_Absynpp_rewrite_temp_tvars=fs->rewrite_temp_tvars;Cyc_Absynpp_print_all_tvars=
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
_dyneither_ptr*v){Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,({
struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->hd=v;_tmp1->tl=0;
_tmp1;}));}static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l){if(((struct
Cyc_List_List*)_check_null(*l))->tl == 0)*l=0;else{Cyc_Absynpp_suppr_last(&((
struct Cyc_List_List*)_check_null(*l))->tl);}}static void Cyc_Absynpp_curr_namespace_drop(){((
void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
struct _dyneither_ptr Cyc_Absynpp_char_escape(char c){switch(c){case '\a': _LL0:
return({const char*_tmp2="\\a";_tag_dyneither(_tmp2,sizeof(char),3);});case '\b':
_LL1: return({const char*_tmp3="\\b";_tag_dyneither(_tmp3,sizeof(char),3);});case '\f':
_LL2: return({const char*_tmp4="\\f";_tag_dyneither(_tmp4,sizeof(char),3);});case '\n':
_LL3: return({const char*_tmp5="\\n";_tag_dyneither(_tmp5,sizeof(char),3);});case '\r':
_LL4: return({const char*_tmp6="\\r";_tag_dyneither(_tmp6,sizeof(char),3);});case '\t':
_LL5: return({const char*_tmp7="\\t";_tag_dyneither(_tmp7,sizeof(char),3);});case '\v':
_LL6: return({const char*_tmp8="\\v";_tag_dyneither(_tmp8,sizeof(char),3);});case '\\':
_LL7: return({const char*_tmp9="\\\\";_tag_dyneither(_tmp9,sizeof(char),3);});case
'"': _LL8: return({const char*_tmpA="\"";_tag_dyneither(_tmpA,sizeof(char),2);});
case '\'': _LL9: return({const char*_tmpB="\\'";_tag_dyneither(_tmpB,sizeof(char),3);});
default: _LLA: if(c >= ' '  && c <= '~'){struct _dyneither_ptr _tmpC=Cyc_Core_new_string(
2);({struct _dyneither_ptr _tmpD=_dyneither_ptr_plus(_tmpC,sizeof(char),0);char
_tmpE=*((char*)_check_dyneither_subscript(_tmpD,sizeof(char),0));char _tmpF=c;if(
_get_dyneither_size(_tmpD,sizeof(char))== 1  && (_tmpE == '\000'  && _tmpF != '\000'))
_throw_arraybounds();*((char*)_tmpD.curr)=_tmpF;});return(struct _dyneither_ptr)
_tmpC;}else{struct _dyneither_ptr _tmp10=Cyc_Core_new_string(5);int j=0;({struct
_dyneither_ptr _tmp11=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++);char _tmp12=*((
char*)_check_dyneither_subscript(_tmp11,sizeof(char),0));char _tmp13='\\';if(
_get_dyneither_size(_tmp11,sizeof(char))== 1  && (_tmp12 == '\000'  && _tmp13 != '\000'))
_throw_arraybounds();*((char*)_tmp11.curr)=_tmp13;});({struct _dyneither_ptr
_tmp14=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++);char _tmp15=*((char*)
_check_dyneither_subscript(_tmp14,sizeof(char),0));char _tmp16=(char)('0' + ((
unsigned char)c >> 6 & 3));if(_get_dyneither_size(_tmp14,sizeof(char))== 1  && (
_tmp15 == '\000'  && _tmp16 != '\000'))_throw_arraybounds();*((char*)_tmp14.curr)=
_tmp16;});({struct _dyneither_ptr _tmp17=_dyneither_ptr_plus(_tmp10,sizeof(char),j
++);char _tmp18=*((char*)_check_dyneither_subscript(_tmp17,sizeof(char),0));char
_tmp19=(char)('0' + (c >> 3 & 7));if(_get_dyneither_size(_tmp17,sizeof(char))== 1
 && (_tmp18 == '\000'  && _tmp19 != '\000'))_throw_arraybounds();*((char*)_tmp17.curr)=
_tmp19;});({struct _dyneither_ptr _tmp1A=_dyneither_ptr_plus(_tmp10,sizeof(char),j
++);char _tmp1B=*((char*)_check_dyneither_subscript(_tmp1A,sizeof(char),0));char
_tmp1C=(char)('0' + (c & 7));if(_get_dyneither_size(_tmp1A,sizeof(char))== 1  && (
_tmp1B == '\000'  && _tmp1C != '\000'))_throw_arraybounds();*((char*)_tmp1A.curr)=
_tmp1C;});return(struct _dyneither_ptr)_tmp10;}}}static int Cyc_Absynpp_special(
struct _dyneither_ptr s){int sz=(int)(_get_dyneither_size(s,sizeof(char))- 1);{int i=
0;for(0;i < sz;++ i){char c=*((const char*)_check_dyneither_subscript(s,sizeof(char),
i));if(((c <= ' '  || c >= '~') || c == '"') || c == '\\')return 1;}}return 0;}struct
_dyneither_ptr Cyc_Absynpp_string_escape(struct _dyneither_ptr s){if(!Cyc_Absynpp_special(
s))return s;{int n=(int)(_get_dyneither_size(s,sizeof(char))- 1);if(n > 0  && *((
const char*)_check_dyneither_subscript(s,sizeof(char),n))== '\000')-- n;{int len=0;{
int i=0;for(0;i <= n;++ i){char _tmp1D=*((const char*)_check_dyneither_subscript(s,
sizeof(char),i));_LLD: if(_tmp1D != '\a')goto _LLF;_LLE: goto _LL10;_LLF: if(_tmp1D != '\b')
goto _LL11;_LL10: goto _LL12;_LL11: if(_tmp1D != '\f')goto _LL13;_LL12: goto _LL14;_LL13:
if(_tmp1D != '\n')goto _LL15;_LL14: goto _LL16;_LL15: if(_tmp1D != '\r')goto _LL17;
_LL16: goto _LL18;_LL17: if(_tmp1D != '\t')goto _LL19;_LL18: goto _LL1A;_LL19: if(_tmp1D
!= '\v')goto _LL1B;_LL1A: goto _LL1C;_LL1B: if(_tmp1D != '\\')goto _LL1D;_LL1C: goto
_LL1E;_LL1D: if(_tmp1D != '"')goto _LL1F;_LL1E: len +=2;goto _LLC;_LL1F:;_LL20: if(
_tmp1D >= ' '  && _tmp1D <= '~')++ len;else{len +=4;}goto _LLC;_LLC:;}}{struct
_dyneither_ptr t=Cyc_Core_new_string((unsigned int)(len + 1));int j=0;{int i=0;for(0;
i <= n;++ i){char _tmp1E=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
_LL22: if(_tmp1E != '\a')goto _LL24;_LL23:({struct _dyneither_ptr _tmp1F=
_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp20=*((char*)
_check_dyneither_subscript(_tmp1F,sizeof(char),0));char _tmp21='\\';if(
_get_dyneither_size(_tmp1F,sizeof(char))== 1  && (_tmp20 == '\000'  && _tmp21 != '\000'))
_throw_arraybounds();*((char*)_tmp1F.curr)=_tmp21;});({struct _dyneither_ptr
_tmp22=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp23=*((char*)
_check_dyneither_subscript(_tmp22,sizeof(char),0));char _tmp24='a';if(
_get_dyneither_size(_tmp22,sizeof(char))== 1  && (_tmp23 == '\000'  && _tmp24 != '\000'))
_throw_arraybounds();*((char*)_tmp22.curr)=_tmp24;});goto _LL21;_LL24: if(_tmp1E != '\b')
goto _LL26;_LL25:({struct _dyneither_ptr _tmp25=_dyneither_ptr_plus(t,sizeof(char),
j ++);char _tmp26=*((char*)_check_dyneither_subscript(_tmp25,sizeof(char),0));char
_tmp27='\\';if(_get_dyneither_size(_tmp25,sizeof(char))== 1  && (_tmp26 == '\000'
 && _tmp27 != '\000'))_throw_arraybounds();*((char*)_tmp25.curr)=_tmp27;});({
struct _dyneither_ptr _tmp28=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp29=*((
char*)_check_dyneither_subscript(_tmp28,sizeof(char),0));char _tmp2A='b';if(
_get_dyneither_size(_tmp28,sizeof(char))== 1  && (_tmp29 == '\000'  && _tmp2A != '\000'))
_throw_arraybounds();*((char*)_tmp28.curr)=_tmp2A;});goto _LL21;_LL26: if(_tmp1E != '\f')
goto _LL28;_LL27:({struct _dyneither_ptr _tmp2B=_dyneither_ptr_plus(t,sizeof(char),
j ++);char _tmp2C=*((char*)_check_dyneither_subscript(_tmp2B,sizeof(char),0));char
_tmp2D='\\';if(_get_dyneither_size(_tmp2B,sizeof(char))== 1  && (_tmp2C == '\000'
 && _tmp2D != '\000'))_throw_arraybounds();*((char*)_tmp2B.curr)=_tmp2D;});({
struct _dyneither_ptr _tmp2E=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp2F=*((
char*)_check_dyneither_subscript(_tmp2E,sizeof(char),0));char _tmp30='f';if(
_get_dyneither_size(_tmp2E,sizeof(char))== 1  && (_tmp2F == '\000'  && _tmp30 != '\000'))
_throw_arraybounds();*((char*)_tmp2E.curr)=_tmp30;});goto _LL21;_LL28: if(_tmp1E != '\n')
goto _LL2A;_LL29:({struct _dyneither_ptr _tmp31=_dyneither_ptr_plus(t,sizeof(char),
j ++);char _tmp32=*((char*)_check_dyneither_subscript(_tmp31,sizeof(char),0));char
_tmp33='\\';if(_get_dyneither_size(_tmp31,sizeof(char))== 1  && (_tmp32 == '\000'
 && _tmp33 != '\000'))_throw_arraybounds();*((char*)_tmp31.curr)=_tmp33;});({
struct _dyneither_ptr _tmp34=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp35=*((
char*)_check_dyneither_subscript(_tmp34,sizeof(char),0));char _tmp36='n';if(
_get_dyneither_size(_tmp34,sizeof(char))== 1  && (_tmp35 == '\000'  && _tmp36 != '\000'))
_throw_arraybounds();*((char*)_tmp34.curr)=_tmp36;});goto _LL21;_LL2A: if(_tmp1E != '\r')
goto _LL2C;_LL2B:({struct _dyneither_ptr _tmp37=_dyneither_ptr_plus(t,sizeof(char),
j ++);char _tmp38=*((char*)_check_dyneither_subscript(_tmp37,sizeof(char),0));char
_tmp39='\\';if(_get_dyneither_size(_tmp37,sizeof(char))== 1  && (_tmp38 == '\000'
 && _tmp39 != '\000'))_throw_arraybounds();*((char*)_tmp37.curr)=_tmp39;});({
struct _dyneither_ptr _tmp3A=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp3B=*((
char*)_check_dyneither_subscript(_tmp3A,sizeof(char),0));char _tmp3C='r';if(
_get_dyneither_size(_tmp3A,sizeof(char))== 1  && (_tmp3B == '\000'  && _tmp3C != '\000'))
_throw_arraybounds();*((char*)_tmp3A.curr)=_tmp3C;});goto _LL21;_LL2C: if(_tmp1E != '\t')
goto _LL2E;_LL2D:({struct _dyneither_ptr _tmp3D=_dyneither_ptr_plus(t,sizeof(char),
j ++);char _tmp3E=*((char*)_check_dyneither_subscript(_tmp3D,sizeof(char),0));char
_tmp3F='\\';if(_get_dyneither_size(_tmp3D,sizeof(char))== 1  && (_tmp3E == '\000'
 && _tmp3F != '\000'))_throw_arraybounds();*((char*)_tmp3D.curr)=_tmp3F;});({
struct _dyneither_ptr _tmp40=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp41=*((
char*)_check_dyneither_subscript(_tmp40,sizeof(char),0));char _tmp42='t';if(
_get_dyneither_size(_tmp40,sizeof(char))== 1  && (_tmp41 == '\000'  && _tmp42 != '\000'))
_throw_arraybounds();*((char*)_tmp40.curr)=_tmp42;});goto _LL21;_LL2E: if(_tmp1E != '\v')
goto _LL30;_LL2F:({struct _dyneither_ptr _tmp43=_dyneither_ptr_plus(t,sizeof(char),
j ++);char _tmp44=*((char*)_check_dyneither_subscript(_tmp43,sizeof(char),0));char
_tmp45='\\';if(_get_dyneither_size(_tmp43,sizeof(char))== 1  && (_tmp44 == '\000'
 && _tmp45 != '\000'))_throw_arraybounds();*((char*)_tmp43.curr)=_tmp45;});({
struct _dyneither_ptr _tmp46=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp47=*((
char*)_check_dyneither_subscript(_tmp46,sizeof(char),0));char _tmp48='v';if(
_get_dyneither_size(_tmp46,sizeof(char))== 1  && (_tmp47 == '\000'  && _tmp48 != '\000'))
_throw_arraybounds();*((char*)_tmp46.curr)=_tmp48;});goto _LL21;_LL30: if(_tmp1E != '\\')
goto _LL32;_LL31:({struct _dyneither_ptr _tmp49=_dyneither_ptr_plus(t,sizeof(char),
j ++);char _tmp4A=*((char*)_check_dyneither_subscript(_tmp49,sizeof(char),0));char
_tmp4B='\\';if(_get_dyneither_size(_tmp49,sizeof(char))== 1  && (_tmp4A == '\000'
 && _tmp4B != '\000'))_throw_arraybounds();*((char*)_tmp49.curr)=_tmp4B;});({
struct _dyneither_ptr _tmp4C=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp4D=*((
char*)_check_dyneither_subscript(_tmp4C,sizeof(char),0));char _tmp4E='\\';if(
_get_dyneither_size(_tmp4C,sizeof(char))== 1  && (_tmp4D == '\000'  && _tmp4E != '\000'))
_throw_arraybounds();*((char*)_tmp4C.curr)=_tmp4E;});goto _LL21;_LL32: if(_tmp1E != '"')
goto _LL34;_LL33:({struct _dyneither_ptr _tmp4F=_dyneither_ptr_plus(t,sizeof(char),
j ++);char _tmp50=*((char*)_check_dyneither_subscript(_tmp4F,sizeof(char),0));char
_tmp51='\\';if(_get_dyneither_size(_tmp4F,sizeof(char))== 1  && (_tmp50 == '\000'
 && _tmp51 != '\000'))_throw_arraybounds();*((char*)_tmp4F.curr)=_tmp51;});({
struct _dyneither_ptr _tmp52=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp53=*((
char*)_check_dyneither_subscript(_tmp52,sizeof(char),0));char _tmp54='"';if(
_get_dyneither_size(_tmp52,sizeof(char))== 1  && (_tmp53 == '\000'  && _tmp54 != '\000'))
_throw_arraybounds();*((char*)_tmp52.curr)=_tmp54;});goto _LL21;_LL34:;_LL35: if(
_tmp1E >= ' '  && _tmp1E <= '~')({struct _dyneither_ptr _tmp55=_dyneither_ptr_plus(t,
sizeof(char),j ++);char _tmp56=*((char*)_check_dyneither_subscript(_tmp55,sizeof(
char),0));char _tmp57=_tmp1E;if(_get_dyneither_size(_tmp55,sizeof(char))== 1  && (
_tmp56 == '\000'  && _tmp57 != '\000'))_throw_arraybounds();*((char*)_tmp55.curr)=
_tmp57;});else{({struct _dyneither_ptr _tmp58=_dyneither_ptr_plus(t,sizeof(char),j
++);char _tmp59=*((char*)_check_dyneither_subscript(_tmp58,sizeof(char),0));char
_tmp5A='\\';if(_get_dyneither_size(_tmp58,sizeof(char))== 1  && (_tmp59 == '\000'
 && _tmp5A != '\000'))_throw_arraybounds();*((char*)_tmp58.curr)=_tmp5A;});({
struct _dyneither_ptr _tmp5B=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp5C=*((
char*)_check_dyneither_subscript(_tmp5B,sizeof(char),0));char _tmp5D=(char)('0' + (
_tmp1E >> 6 & 7));if(_get_dyneither_size(_tmp5B,sizeof(char))== 1  && (_tmp5C == '\000'
 && _tmp5D != '\000'))_throw_arraybounds();*((char*)_tmp5B.curr)=_tmp5D;});({
struct _dyneither_ptr _tmp5E=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp5F=*((
char*)_check_dyneither_subscript(_tmp5E,sizeof(char),0));char _tmp60=(char)('0' + (
_tmp1E >> 3 & 7));if(_get_dyneither_size(_tmp5E,sizeof(char))== 1  && (_tmp5F == '\000'
 && _tmp60 != '\000'))_throw_arraybounds();*((char*)_tmp5E.curr)=_tmp60;});({
struct _dyneither_ptr _tmp61=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp62=*((
char*)_check_dyneither_subscript(_tmp61,sizeof(char),0));char _tmp63=(char)('0' + (
_tmp1E & 7));if(_get_dyneither_size(_tmp61,sizeof(char))== 1  && (_tmp62 == '\000'
 && _tmp63 != '\000'))_throw_arraybounds();*((char*)_tmp61.curr)=_tmp63;});}goto
_LL21;_LL21:;}}return(struct _dyneither_ptr)t;};};};}static char _tmp64[9]="restrict";
static struct _dyneither_ptr Cyc_Absynpp_restrict_string={_tmp64,_tmp64,_tmp64 + 9};
static char _tmp65[9]="volatile";static struct _dyneither_ptr Cyc_Absynpp_volatile_string={
_tmp65,_tmp65,_tmp65 + 9};static char _tmp66[6]="const";static struct _dyneither_ptr
Cyc_Absynpp_const_str={_tmp66,_tmp66,_tmp66 + 6};static struct _dyneither_ptr*Cyc_Absynpp_restrict_sp=&
Cyc_Absynpp_restrict_string;static struct _dyneither_ptr*Cyc_Absynpp_volatile_sp=&
Cyc_Absynpp_volatile_string;static struct _dyneither_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){struct Cyc_List_List*
l=0;if(tq.q_restrict)l=({struct Cyc_List_List*_tmp67=_cycalloc(sizeof(*_tmp67));
_tmp67->hd=Cyc_Absynpp_restrict_sp;_tmp67->tl=l;_tmp67;});if(tq.q_volatile)l=({
struct Cyc_List_List*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->hd=Cyc_Absynpp_volatile_sp;
_tmp68->tl=l;_tmp68;});if(tq.print_const)l=({struct Cyc_List_List*_tmp69=
_cycalloc(sizeof(*_tmp69));_tmp69->hd=Cyc_Absynpp_const_sp;_tmp69->tl=l;_tmp69;});
return Cyc_PP_egroup(({const char*_tmp6A="";_tag_dyneither(_tmp6A,sizeof(char),1);}),({
const char*_tmp6B=" ";_tag_dyneither(_tmp6B,sizeof(char),2);}),({const char*_tmp6C=" ";
_tag_dyneither(_tmp6C,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));}
struct _dyneither_ptr Cyc_Absynpp_kind2string(enum Cyc_Absyn_Kind k){switch(k){case
Cyc_Absyn_AnyKind: _LL36: return({const char*_tmp6D="A";_tag_dyneither(_tmp6D,
sizeof(char),2);});case Cyc_Absyn_MemKind: _LL37: return({const char*_tmp6E="M";
_tag_dyneither(_tmp6E,sizeof(char),2);});case Cyc_Absyn_BoxKind: _LL38: return({
const char*_tmp6F="B";_tag_dyneither(_tmp6F,sizeof(char),2);});case Cyc_Absyn_RgnKind:
_LL39: return({const char*_tmp70="R";_tag_dyneither(_tmp70,sizeof(char),2);});case
Cyc_Absyn_UniqueRgnKind: _LL3A: return({const char*_tmp71="UR";_tag_dyneither(
_tmp71,sizeof(char),3);});case Cyc_Absyn_TopRgnKind: _LL3B: return({const char*
_tmp72="TR";_tag_dyneither(_tmp72,sizeof(char),3);});case Cyc_Absyn_EffKind: _LL3C:
return({const char*_tmp73="E";_tag_dyneither(_tmp73,sizeof(char),2);});case Cyc_Absyn_IntKind:
_LL3D: return({const char*_tmp74="I";_tag_dyneither(_tmp74,sizeof(char),2);});}}
struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(enum Cyc_Absyn_Kind k){return Cyc_PP_text(
Cyc_Absynpp_kind2string(k));}struct _dyneither_ptr Cyc_Absynpp_kindbound2string(
void*c){void*_tmp75=Cyc_Absyn_compress_kb(c);enum Cyc_Absyn_Kind _tmp77;enum Cyc_Absyn_Kind
_tmp7A;_LL40: {struct Cyc_Absyn_Eq_kb_struct*_tmp76=(struct Cyc_Absyn_Eq_kb_struct*)
_tmp75;if(_tmp76->tag != 0)goto _LL42;else{_tmp77=_tmp76->f1;}}_LL41: return Cyc_Absynpp_kind2string(
_tmp77);_LL42: {struct Cyc_Absyn_Unknown_kb_struct*_tmp78=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp75;if(_tmp78->tag != 1)goto _LL44;}_LL43: if(Cyc_PP_tex_output)return({const
char*_tmp7B="{?}";_tag_dyneither(_tmp7B,sizeof(char),4);});else{return({const
char*_tmp7C="?";_tag_dyneither(_tmp7C,sizeof(char),2);});}_LL44: {struct Cyc_Absyn_Less_kb_struct*
_tmp79=(struct Cyc_Absyn_Less_kb_struct*)_tmp75;if(_tmp79->tag != 2)goto _LL3F;
else{_tmp7A=_tmp79->f2;}}_LL45: return Cyc_Absynpp_kind2string(_tmp7A);_LL3F:;}
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){void*_tmp7D=Cyc_Absyn_compress_kb(
c);enum Cyc_Absyn_Kind _tmp7F;enum Cyc_Absyn_Kind _tmp82;_LL47: {struct Cyc_Absyn_Eq_kb_struct*
_tmp7E=(struct Cyc_Absyn_Eq_kb_struct*)_tmp7D;if(_tmp7E->tag != 0)goto _LL49;else{
_tmp7F=_tmp7E->f1;}}_LL48: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp7F));
_LL49: {struct Cyc_Absyn_Unknown_kb_struct*_tmp80=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp7D;if(_tmp80->tag != 1)goto _LL4B;}_LL4A: if(Cyc_PP_tex_output)return Cyc_PP_text_width(({
const char*_tmp83="{?}";_tag_dyneither(_tmp83,sizeof(char),4);}),1);else{return
Cyc_PP_text(({const char*_tmp84="?";_tag_dyneither(_tmp84,sizeof(char),2);}));}
_LL4B: {struct Cyc_Absyn_Less_kb_struct*_tmp81=(struct Cyc_Absyn_Less_kb_struct*)
_tmp7D;if(_tmp81->tag != 2)goto _LL46;else{_tmp82=_tmp81->f2;}}_LL4C: return Cyc_PP_text(
Cyc_Absynpp_kind2string(_tmp82));_LL46:;}struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(
struct Cyc_List_List*ts){return Cyc_PP_egroup(({const char*_tmp85="<";
_tag_dyneither(_tmp85,sizeof(char),2);}),({const char*_tmp86=">";_tag_dyneither(
_tmp86,sizeof(char),2);}),({const char*_tmp87=",";_tag_dyneither(_tmp87,sizeof(
char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_typ2doc,ts));}struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(
struct Cyc_Absyn_Tvar*tv){void*_tmp88=Cyc_Absyn_compress_kb(tv->kind);enum Cyc_Absyn_Kind
_tmp8B;enum Cyc_Absyn_Kind _tmp8D;enum Cyc_Absyn_Kind _tmp8F;_LL4E: {struct Cyc_Absyn_Unknown_kb_struct*
_tmp89=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp88;if(_tmp89->tag != 1)goto _LL50;}
_LL4F: goto _LL51;_LL50: {struct Cyc_Absyn_Eq_kb_struct*_tmp8A=(struct Cyc_Absyn_Eq_kb_struct*)
_tmp88;if(_tmp8A->tag != 0)goto _LL52;else{_tmp8B=_tmp8A->f1;if(_tmp8B != Cyc_Absyn_BoxKind)
goto _LL52;}}_LL51: return Cyc_PP_textptr(tv->name);_LL52: {struct Cyc_Absyn_Less_kb_struct*
_tmp8C=(struct Cyc_Absyn_Less_kb_struct*)_tmp88;if(_tmp8C->tag != 2)goto _LL54;
else{_tmp8D=_tmp8C->f2;}}_LL53: _tmp8F=_tmp8D;goto _LL55;_LL54: {struct Cyc_Absyn_Eq_kb_struct*
_tmp8E=(struct Cyc_Absyn_Eq_kb_struct*)_tmp88;if(_tmp8E->tag != 0)goto _LL4D;else{
_tmp8F=_tmp8E->f1;}}_LL55: return({struct Cyc_PP_Doc*_tmp90[3];_tmp90[2]=Cyc_Absynpp_kind2doc(
_tmp8F);_tmp90[1]=Cyc_PP_text(({const char*_tmp91="::";_tag_dyneither(_tmp91,
sizeof(char),3);}));_tmp90[0]=Cyc_PP_textptr(tv->name);Cyc_PP_cat(_tag_dyneither(
_tmp90,sizeof(struct Cyc_PP_Doc*),3));});_LL4D:;}struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(
struct Cyc_List_List*tvs){return Cyc_PP_egroup(({const char*_tmp92="<";
_tag_dyneither(_tmp92,sizeof(char),2);}),({const char*_tmp93=">";_tag_dyneither(
_tmp93,sizeof(char),2);}),({const char*_tmp94=",";_tag_dyneither(_tmp94,sizeof(
char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));}static struct
_dyneither_ptr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv){return tv->name;}
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);return Cyc_PP_egroup(({const char*_tmp95="<";
_tag_dyneither(_tmp95,sizeof(char),2);}),({const char*_tmp96=">";_tag_dyneither(
_tmp96,sizeof(char),2);}),({const char*_tmp97=",";_tag_dyneither(_tmp97,sizeof(
char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(
struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_get_name,tvs)));}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple15*t){return Cyc_Absynpp_tqtd2doc((*
t).f1,(*t).f2,0);}struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
return Cyc_PP_group(({const char*_tmp98="(";_tag_dyneither(_tmp98,sizeof(char),2);}),({
const char*_tmp99=")";_tag_dyneither(_tmp99,sizeof(char),2);}),({const char*_tmp9A=",";
_tag_dyneither(_tmp9A,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}
struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){void*_tmp9B=att;_LL57: {
struct Cyc_Absyn_Stdcall_att_struct*_tmp9C=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmp9B;if(_tmp9C->tag != 1)goto _LL59;}_LL58: goto _LL5A;_LL59: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp9D=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp9B;if(_tmp9D->tag != 2)goto _LL5B;}
_LL5A: goto _LL5C;_LL5B: {struct Cyc_Absyn_Fastcall_att_struct*_tmp9E=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp9B;if(_tmp9E->tag != 3)goto _LL5D;}_LL5C: return Cyc_PP_nil_doc();_LL5D:;_LL5E:
return Cyc_PP_text(Cyc_Absyn_attribute2string(att));_LL56:;}struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(
struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){void*_tmp9F=(void*)atts->hd;
_LL60: {struct Cyc_Absyn_Stdcall_att_struct*_tmpA0=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmp9F;if(_tmpA0->tag != 1)goto _LL62;}_LL61: return Cyc_PP_text(({const char*_tmpA3=" _stdcall ";
_tag_dyneither(_tmpA3,sizeof(char),11);}));_LL62: {struct Cyc_Absyn_Cdecl_att_struct*
_tmpA1=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp9F;if(_tmpA1->tag != 2)goto _LL64;}
_LL63: return Cyc_PP_text(({const char*_tmpA4=" _cdecl ";_tag_dyneither(_tmpA4,
sizeof(char),9);}));_LL64: {struct Cyc_Absyn_Fastcall_att_struct*_tmpA2=(struct
Cyc_Absyn_Fastcall_att_struct*)_tmp9F;if(_tmpA2->tag != 3)goto _LL66;}_LL65: return
Cyc_PP_text(({const char*_tmpA5=" _fastcall ";_tag_dyneither(_tmpA5,sizeof(char),
12);}));_LL66:;_LL67: goto _LL5F;_LL5F:;}return Cyc_PP_nil_doc();}struct Cyc_PP_Doc*
Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){int hasatt=0;{struct Cyc_List_List*
atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){void*_tmpA6=(void*)atts2->hd;_LL69: {
struct Cyc_Absyn_Stdcall_att_struct*_tmpA7=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmpA6;if(_tmpA7->tag != 1)goto _LL6B;}_LL6A: goto _LL6C;_LL6B: {struct Cyc_Absyn_Cdecl_att_struct*
_tmpA8=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpA6;if(_tmpA8->tag != 2)goto _LL6D;}
_LL6C: goto _LL6E;_LL6D: {struct Cyc_Absyn_Fastcall_att_struct*_tmpA9=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmpA6;if(_tmpA9->tag != 3)goto _LL6F;}_LL6E: goto _LL68;_LL6F:;_LL70: hasatt=1;goto
_LL68;_LL68:;}}if(!hasatt)return Cyc_PP_nil_doc();return({struct Cyc_PP_Doc*_tmpAA[
3];_tmpAA[2]=Cyc_PP_text(({const char*_tmpAF=")";_tag_dyneither(_tmpAF,sizeof(
char),2);}));_tmpAA[1]=Cyc_PP_group(({const char*_tmpAC="";_tag_dyneither(_tmpAC,
sizeof(char),1);}),({const char*_tmpAD="";_tag_dyneither(_tmpAD,sizeof(char),1);}),({
const char*_tmpAE=" ";_tag_dyneither(_tmpAE,sizeof(char),2);}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,
atts));_tmpAA[0]=Cyc_PP_text(({const char*_tmpAB=" __declspec(";_tag_dyneither(
_tmpAB,sizeof(char),13);}));Cyc_PP_cat(_tag_dyneither(_tmpAA,sizeof(struct Cyc_PP_Doc*),
3));});}struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){return Cyc_PP_text(Cyc_Absyn_attribute2string(
a));}struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){if(atts == 0)
return Cyc_PP_nil_doc();switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL71:
return Cyc_Absynpp_noncallconv2doc(atts);case Cyc_Cyclone_Gcc_c: _LL72: return({
struct Cyc_PP_Doc*_tmpB0[2];_tmpB0[1]=Cyc_PP_group(({const char*_tmpB2="((";
_tag_dyneither(_tmpB2,sizeof(char),3);}),({const char*_tmpB3="))";_tag_dyneither(
_tmpB3,sizeof(char),3);}),({const char*_tmpB4=",";_tag_dyneither(_tmpB4,sizeof(
char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts));_tmpB0[0]=Cyc_PP_text(({const char*
_tmpB1=" __attribute__";_tag_dyneither(_tmpB1,sizeof(char),15);}));Cyc_PP_cat(
_tag_dyneither(_tmpB0,sizeof(struct Cyc_PP_Doc*),2));});}}int Cyc_Absynpp_next_is_pointer(
struct Cyc_List_List*tms){if(tms == 0)return 0;{void*_tmpB5=(void*)tms->hd;_LL75: {
struct Cyc_Absyn_Pointer_mod_struct*_tmpB6=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmpB5;if(_tmpB6->tag != 2)goto _LL77;}_LL76: return 1;_LL77: {struct Cyc_Absyn_Attributes_mod_struct*
_tmpB7=(struct Cyc_Absyn_Attributes_mod_struct*)_tmpB5;if(_tmpB7->tag != 5)goto
_LL79;}_LL78: switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL7B: return 0;
default: _LL7C: return Cyc_Absynpp_next_is_pointer(tms->tl);}_LL79:;_LL7A: return 0;
_LL74:;};}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);static struct Cyc_PP_Doc*
Cyc_Absynpp_cache_question=0;static struct Cyc_PP_Doc*Cyc_Absynpp_question(){if(!((
unsigned int)Cyc_Absynpp_cache_question)){if(Cyc_PP_tex_output)Cyc_Absynpp_cache_question=(
struct Cyc_PP_Doc*)Cyc_PP_text_width(({const char*_tmpB8="{?}";_tag_dyneither(
_tmpB8,sizeof(char),4);}),1);else{Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)
Cyc_PP_text(({const char*_tmpB9="?";_tag_dyneither(_tmpB9,sizeof(char),2);}));}}
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_question);}static struct
Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){if(!((
unsigned int)Cyc_Absynpp_cache_lb)){if(Cyc_PP_tex_output)Cyc_Absynpp_cache_lb=(
struct Cyc_PP_Doc*)Cyc_PP_text_width(({const char*_tmpBA="{\\lb}";_tag_dyneither(
_tmpBA,sizeof(char),6);}),1);else{Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text(({
const char*_tmpBB="{";_tag_dyneither(_tmpBB,sizeof(char),2);}));}}return(struct
Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=
0;static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){if(!((unsigned int)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output)Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(({
const char*_tmpBC="{\\rb}";_tag_dyneither(_tmpBC,sizeof(char),6);}),1);else{Cyc_Absynpp_cache_rb=(
struct Cyc_PP_Doc*)Cyc_PP_text(({const char*_tmpBD="}";_tag_dyneither(_tmpBD,
sizeof(char),2);}));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_rb);}
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){if(Cyc_PP_tex_output)Cyc_Absynpp_cache_dollar=(
struct Cyc_PP_Doc*)Cyc_PP_text_width(({const char*_tmpBE="\\$";_tag_dyneither(
_tmpBE,sizeof(char),3);}),1);else{Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)
Cyc_PP_text(({const char*_tmpBF="$";_tag_dyneither(_tmpBF,sizeof(char),2);}));}}
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}struct Cyc_PP_Doc*
Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*ss){return({
struct Cyc_PP_Doc*_tmpC0[3];_tmpC0[2]=Cyc_Absynpp_rb();_tmpC0[1]=Cyc_PP_seq(sep,
ss);_tmpC0[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_dyneither(_tmpC0,sizeof(struct Cyc_PP_Doc*),
3));});}static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){for(0;tms != 0;
tms=tms->tl){void*_tmpC1=(void*)tms->hd;void*_tmpC5;struct Cyc_List_List*_tmpC7;
_LL7F: {struct Cyc_Absyn_Carray_mod_struct*_tmpC2=(struct Cyc_Absyn_Carray_mod_struct*)
_tmpC1;if(_tmpC2->tag != 0)goto _LL81;}_LL80:({void*_tmpCC=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmpCD="Carray_mod ";_tag_dyneither(_tmpCD,sizeof(char),12);}),
_tag_dyneither(_tmpCC,sizeof(void*),0));});goto _LL7E;_LL81: {struct Cyc_Absyn_ConstArray_mod_struct*
_tmpC3=(struct Cyc_Absyn_ConstArray_mod_struct*)_tmpC1;if(_tmpC3->tag != 1)goto
_LL83;}_LL82:({void*_tmpCE=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpCF="ConstArray_mod ";
_tag_dyneither(_tmpCF,sizeof(char),16);}),_tag_dyneither(_tmpCE,sizeof(void*),0));});
goto _LL7E;_LL83: {struct Cyc_Absyn_Function_mod_struct*_tmpC4=(struct Cyc_Absyn_Function_mod_struct*)
_tmpC1;if(_tmpC4->tag != 3)goto _LL85;else{_tmpC5=(void*)_tmpC4->f1;{struct Cyc_Absyn_WithTypes_struct*
_tmpC6=(struct Cyc_Absyn_WithTypes_struct*)_tmpC5;if(_tmpC6->tag != 1)goto _LL85;
else{_tmpC7=_tmpC6->f1;}};}}_LL84:({void*_tmpD0=0;Cyc_fprintf(Cyc_stderr,({const
char*_tmpD1="Function_mod(";_tag_dyneither(_tmpD1,sizeof(char),14);}),
_tag_dyneither(_tmpD0,sizeof(void*),0));});for(0;_tmpC7 != 0;_tmpC7=_tmpC7->tl){
struct Cyc_Core_Opt*_tmpD2=(*((struct _tuple7*)_tmpC7->hd)).f1;if(_tmpD2 == 0)({
void*_tmpD3=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpD4="?";_tag_dyneither(
_tmpD4,sizeof(char),2);}),_tag_dyneither(_tmpD3,sizeof(void*),0));});else{({void*
_tmpD5=0;Cyc_fprintf(Cyc_stderr,*((struct _dyneither_ptr*)_tmpD2->v),
_tag_dyneither(_tmpD5,sizeof(void*),0));});}if(_tmpC7->tl != 0)({void*_tmpD6=0;
Cyc_fprintf(Cyc_stderr,({const char*_tmpD7=",";_tag_dyneither(_tmpD7,sizeof(char),
2);}),_tag_dyneither(_tmpD6,sizeof(void*),0));});}({void*_tmpD8=0;Cyc_fprintf(
Cyc_stderr,({const char*_tmpD9=") ";_tag_dyneither(_tmpD9,sizeof(char),3);}),
_tag_dyneither(_tmpD8,sizeof(void*),0));});goto _LL7E;_LL85: {struct Cyc_Absyn_Function_mod_struct*
_tmpC8=(struct Cyc_Absyn_Function_mod_struct*)_tmpC1;if(_tmpC8->tag != 3)goto _LL87;}
_LL86:({void*_tmpDA=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpDB="Function_mod()";
_tag_dyneither(_tmpDB,sizeof(char),15);}),_tag_dyneither(_tmpDA,sizeof(void*),0));});
goto _LL7E;_LL87: {struct Cyc_Absyn_Attributes_mod_struct*_tmpC9=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpC1;if(_tmpC9->tag != 5)goto _LL89;}_LL88:({void*_tmpDC=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmpDD="Attributes_mod ";_tag_dyneither(_tmpDD,sizeof(char),16);}),
_tag_dyneither(_tmpDC,sizeof(void*),0));});goto _LL7E;_LL89: {struct Cyc_Absyn_TypeParams_mod_struct*
_tmpCA=(struct Cyc_Absyn_TypeParams_mod_struct*)_tmpC1;if(_tmpCA->tag != 4)goto
_LL8B;}_LL8A:({void*_tmpDE=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpDF="TypeParams_mod ";
_tag_dyneither(_tmpDF,sizeof(char),16);}),_tag_dyneither(_tmpDE,sizeof(void*),0));});
goto _LL7E;_LL8B: {struct Cyc_Absyn_Pointer_mod_struct*_tmpCB=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmpC1;if(_tmpCB->tag != 2)goto _LL7E;}_LL8C:({void*_tmpE0=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmpE1="Pointer_mod ";_tag_dyneither(_tmpE1,sizeof(char),13);}),
_tag_dyneither(_tmpE0,sizeof(void*),0));});goto _LL7E;_LL7E:;}({void*_tmpE2=0;Cyc_fprintf(
Cyc_stderr,({const char*_tmpE3="\n";_tag_dyneither(_tmpE3,sizeof(char),2);}),
_tag_dyneither(_tmpE2,sizeof(void*),0));});}struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(
int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);struct Cyc_PP_Doc*p_rest=({
struct Cyc_PP_Doc*_tmp128[3];_tmp128[2]=Cyc_PP_text(({const char*_tmp12A=")";
_tag_dyneither(_tmp12A,sizeof(char),2);}));_tmp128[1]=rest;_tmp128[0]=Cyc_PP_text(({
const char*_tmp129="(";_tag_dyneither(_tmp129,sizeof(char),2);}));Cyc_PP_cat(
_tag_dyneither(_tmp128,sizeof(struct Cyc_PP_Doc*),3));});void*_tmpE4=(void*)tms->hd;
union Cyc_Absyn_Constraint*_tmpE6;struct Cyc_Absyn_Exp*_tmpE8;union Cyc_Absyn_Constraint*
_tmpE9;void*_tmpEB;struct Cyc_List_List*_tmpED;struct Cyc_List_List*_tmpEF;struct
Cyc_Position_Segment*_tmpF0;int _tmpF1;struct Cyc_Absyn_PtrAtts _tmpF3;void*_tmpF4;
union Cyc_Absyn_Constraint*_tmpF5;union Cyc_Absyn_Constraint*_tmpF6;union Cyc_Absyn_Constraint*
_tmpF7;struct Cyc_Absyn_Tqual _tmpF8;_LL8E: {struct Cyc_Absyn_Carray_mod_struct*
_tmpE5=(struct Cyc_Absyn_Carray_mod_struct*)_tmpE4;if(_tmpE5->tag != 0)goto _LL90;
else{_tmpE6=_tmpE5->f1;}}_LL8F: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=
p_rest;return({struct Cyc_PP_Doc*_tmpF9[2];_tmpF9[1]=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmpE6)?Cyc_PP_text(({const char*_tmpFA="[]@zeroterm";
_tag_dyneither(_tmpFA,sizeof(char),12);})): Cyc_PP_text(({const char*_tmpFB="[]";
_tag_dyneither(_tmpFB,sizeof(char),3);}));_tmpF9[0]=rest;Cyc_PP_cat(
_tag_dyneither(_tmpF9,sizeof(struct Cyc_PP_Doc*),2));});_LL90: {struct Cyc_Absyn_ConstArray_mod_struct*
_tmpE7=(struct Cyc_Absyn_ConstArray_mod_struct*)_tmpE4;if(_tmpE7->tag != 1)goto
_LL92;else{_tmpE8=_tmpE7->f1;_tmpE9=_tmpE7->f2;}}_LL91: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;return({struct Cyc_PP_Doc*_tmpFC[4];_tmpFC[3]=((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpE9)?Cyc_PP_text(({const
char*_tmpFE="]@zeroterm";_tag_dyneither(_tmpFE,sizeof(char),11);})): Cyc_PP_text(({
const char*_tmpFF="]";_tag_dyneither(_tmpFF,sizeof(char),2);}));_tmpFC[2]=Cyc_Absynpp_exp2doc(
_tmpE8);_tmpFC[1]=Cyc_PP_text(({const char*_tmpFD="[";_tag_dyneither(_tmpFD,
sizeof(char),2);}));_tmpFC[0]=rest;Cyc_PP_cat(_tag_dyneither(_tmpFC,sizeof(
struct Cyc_PP_Doc*),4));});_LL92: {struct Cyc_Absyn_Function_mod_struct*_tmpEA=(
struct Cyc_Absyn_Function_mod_struct*)_tmpE4;if(_tmpEA->tag != 3)goto _LL94;else{
_tmpEB=(void*)_tmpEA->f1;}}_LL93: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=
p_rest;{void*_tmp100=_tmpEB;struct Cyc_List_List*_tmp102;int _tmp103;struct Cyc_Absyn_VarargInfo*
_tmp104;struct Cyc_Core_Opt*_tmp105;struct Cyc_List_List*_tmp106;struct Cyc_List_List*
_tmp108;struct Cyc_Position_Segment*_tmp109;_LL9B: {struct Cyc_Absyn_WithTypes_struct*
_tmp101=(struct Cyc_Absyn_WithTypes_struct*)_tmp100;if(_tmp101->tag != 1)goto _LL9D;
else{_tmp102=_tmp101->f1;_tmp103=_tmp101->f2;_tmp104=_tmp101->f3;_tmp105=_tmp101->f4;
_tmp106=_tmp101->f5;}}_LL9C: return({struct Cyc_PP_Doc*_tmp10A[2];_tmp10A[1]=Cyc_Absynpp_funargs2doc(
_tmp102,_tmp103,_tmp104,_tmp105,_tmp106);_tmp10A[0]=rest;Cyc_PP_cat(
_tag_dyneither(_tmp10A,sizeof(struct Cyc_PP_Doc*),2));});_LL9D: {struct Cyc_Absyn_NoTypes_struct*
_tmp107=(struct Cyc_Absyn_NoTypes_struct*)_tmp100;if(_tmp107->tag != 0)goto _LL9A;
else{_tmp108=_tmp107->f1;_tmp109=_tmp107->f2;}}_LL9E: return({struct Cyc_PP_Doc*
_tmp10B[2];_tmp10B[1]=Cyc_PP_group(({const char*_tmp10C="(";_tag_dyneither(
_tmp10C,sizeof(char),2);}),({const char*_tmp10D=")";_tag_dyneither(_tmp10D,
sizeof(char),2);}),({const char*_tmp10E=",";_tag_dyneither(_tmp10E,sizeof(char),2);}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,_tmp108));_tmp10B[0]=rest;Cyc_PP_cat(
_tag_dyneither(_tmp10B,sizeof(struct Cyc_PP_Doc*),2));});_LL9A:;};_LL94: {struct
Cyc_Absyn_Attributes_mod_struct*_tmpEC=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpE4;if(_tmpEC->tag != 5)goto _LL96;else{_tmpED=_tmpEC->f2;}}_LL95: switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LL9F: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmp10F[2];_tmp10F[1]=Cyc_Absynpp_atts2doc(_tmpED);
_tmp10F[0]=rest;Cyc_PP_cat(_tag_dyneither(_tmp10F,sizeof(struct Cyc_PP_Doc*),2));});
case Cyc_Cyclone_Vc_c: _LLA0: if(Cyc_Absynpp_next_is_pointer(tms->tl))return({
struct Cyc_PP_Doc*_tmp110[2];_tmp110[1]=rest;_tmp110[0]=Cyc_Absynpp_callconv2doc(
_tmpED);Cyc_PP_cat(_tag_dyneither(_tmp110,sizeof(struct Cyc_PP_Doc*),2));});
return rest;}_LL96: {struct Cyc_Absyn_TypeParams_mod_struct*_tmpEE=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpE4;if(_tmpEE->tag != 4)goto _LL98;else{_tmpEF=_tmpEE->f1;_tmpF0=_tmpEE->f2;
_tmpF1=_tmpEE->f3;}}_LL97: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmpF1)return({struct Cyc_PP_Doc*_tmp111[2];_tmp111[1]=Cyc_Absynpp_ktvars2doc(
_tmpEF);_tmp111[0]=rest;Cyc_PP_cat(_tag_dyneither(_tmp111,sizeof(struct Cyc_PP_Doc*),
2));});else{return({struct Cyc_PP_Doc*_tmp112[2];_tmp112[1]=Cyc_Absynpp_tvars2doc(
_tmpEF);_tmp112[0]=rest;Cyc_PP_cat(_tag_dyneither(_tmp112,sizeof(struct Cyc_PP_Doc*),
2));});}_LL98: {struct Cyc_Absyn_Pointer_mod_struct*_tmpF2=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmpE4;if(_tmpF2->tag != 2)goto _LL8D;else{_tmpF3=_tmpF2->f1;_tmpF4=_tmpF3.rgn;
_tmpF5=_tmpF3.nullable;_tmpF6=_tmpF3.bounds;_tmpF7=_tmpF3.zero_term;_tmpF8=
_tmpF2->f2;}}_LL99: {struct Cyc_PP_Doc*ptr;{void*_tmp113=((void*(*)(void*y,union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmpF6);
struct Cyc_Absyn_Exp*_tmp116;_LLA3: {struct Cyc_Absyn_DynEither_b_struct*_tmp114=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp113;if(_tmp114->tag != 0)goto _LLA5;}_LLA4:
ptr=Cyc_Absynpp_question();goto _LLA2;_LLA5: {struct Cyc_Absyn_Upper_b_struct*
_tmp115=(struct Cyc_Absyn_Upper_b_struct*)_tmp113;if(_tmp115->tag != 1)goto _LLA2;
else{_tmp116=_tmp115->f1;}}_LLA6: ptr=Cyc_PP_text(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(1,_tmpF5)?({const char*_tmp117="*";_tag_dyneither(
_tmp117,sizeof(char),2);}):({const char*_tmp118="@";_tag_dyneither(_tmp118,
sizeof(char),2);}));{unsigned int _tmp11A;int _tmp11B;struct _tuple10 _tmp119=Cyc_Evexp_eval_const_uint_exp(
_tmp116);_tmp11A=_tmp119.f1;_tmp11B=_tmp119.f2;if(!_tmp11B  || _tmp11A != 1)ptr=({
struct Cyc_PP_Doc*_tmp11C[4];_tmp11C[3]=Cyc_Absynpp_rb();_tmp11C[2]=Cyc_Absynpp_exp2doc(
_tmp116);_tmp11C[1]=Cyc_Absynpp_lb();_tmp11C[0]=ptr;Cyc_PP_cat(_tag_dyneither(
_tmp11C,sizeof(struct Cyc_PP_Doc*),4));});goto _LLA2;};_LLA2:;}if(Cyc_Absynpp_print_zeroterm){
if(!is_char_ptr  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmpF7))ptr=({struct Cyc_PP_Doc*_tmp11D[2];_tmp11D[1]=Cyc_PP_text(({const char*
_tmp11E="@zeroterm ";_tag_dyneither(_tmp11E,sizeof(char),11);}));_tmp11D[0]=ptr;
Cyc_PP_cat(_tag_dyneither(_tmp11D,sizeof(struct Cyc_PP_Doc*),2));});else{if(
is_char_ptr  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmpF7))ptr=({struct Cyc_PP_Doc*_tmp11F[2];_tmp11F[1]=Cyc_PP_text(({const char*
_tmp120="@nozeroterm ";_tag_dyneither(_tmp120,sizeof(char),13);}));_tmp11F[0]=
ptr;Cyc_PP_cat(_tag_dyneither(_tmp11F,sizeof(struct Cyc_PP_Doc*),2));});}}{void*
_tmp121=Cyc_Tcutil_compress(_tmpF4);_LLA8: {struct Cyc_Absyn_HeapRgn_struct*
_tmp122=(struct Cyc_Absyn_HeapRgn_struct*)_tmp121;if(_tmp122->tag != 21)goto _LLAA;}
_LLA9: goto _LLA7;_LLAA:{struct Cyc_Absyn_Evar_struct*_tmp123=(struct Cyc_Absyn_Evar_struct*)
_tmp121;if(_tmp123->tag != 1)goto _LLAC;}if(!Cyc_Absynpp_print_for_cycdoc)goto
_LLAC;_LLAB: goto _LLA7;_LLAC:;_LLAD: ptr=({struct Cyc_PP_Doc*_tmp124[3];_tmp124[2]=
Cyc_PP_text(({const char*_tmp125=" ";_tag_dyneither(_tmp125,sizeof(char),2);}));
_tmp124[1]=Cyc_Absynpp_typ2doc(_tmpF4);_tmp124[0]=ptr;Cyc_PP_cat(_tag_dyneither(
_tmp124,sizeof(struct Cyc_PP_Doc*),3));});_LLA7:;}ptr=({struct Cyc_PP_Doc*_tmp126[
2];_tmp126[1]=Cyc_Absynpp_tqual2doc(_tmpF8);_tmp126[0]=ptr;Cyc_PP_cat(
_tag_dyneither(_tmp126,sizeof(struct Cyc_PP_Doc*),2));});return({struct Cyc_PP_Doc*
_tmp127[2];_tmp127[1]=rest;_tmp127[0]=ptr;Cyc_PP_cat(_tag_dyneither(_tmp127,
sizeof(struct Cyc_PP_Doc*),2));});}_LL8D:;};}struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(
void*t){void*_tmp12B=Cyc_Tcutil_compress(t);_LLAF: {struct Cyc_Absyn_HeapRgn_struct*
_tmp12C=(struct Cyc_Absyn_HeapRgn_struct*)_tmp12B;if(_tmp12C->tag != 21)goto _LLB1;}
_LLB0: return Cyc_PP_text(({const char*_tmp12E="`H";_tag_dyneither(_tmp12E,sizeof(
char),3);}));_LLB1: {struct Cyc_Absyn_UniqueRgn_struct*_tmp12D=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp12B;if(_tmp12D->tag != 22)goto _LLB3;}_LLB2: return Cyc_PP_text(({const char*
_tmp12F="`U";_tag_dyneither(_tmp12F,sizeof(char),3);}));_LLB3:;_LLB4: return Cyc_Absynpp_ntyp2doc(
t);_LLAE:;}static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct
Cyc_List_List**effects,void*t){void*_tmp130=Cyc_Tcutil_compress(t);void*_tmp132;
struct Cyc_List_List*_tmp134;_LLB6: {struct Cyc_Absyn_AccessEff_struct*_tmp131=(
struct Cyc_Absyn_AccessEff_struct*)_tmp130;if(_tmp131->tag != 23)goto _LLB8;else{
_tmp132=(void*)_tmp131->f1;}}_LLB7:*rgions=({struct Cyc_List_List*_tmp135=
_cycalloc(sizeof(*_tmp135));_tmp135->hd=Cyc_Absynpp_rgn2doc(_tmp132);_tmp135->tl=*
rgions;_tmp135;});goto _LLB5;_LLB8: {struct Cyc_Absyn_JoinEff_struct*_tmp133=(
struct Cyc_Absyn_JoinEff_struct*)_tmp130;if(_tmp133->tag != 24)goto _LLBA;else{
_tmp134=_tmp133->f1;}}_LLB9: for(0;_tmp134 != 0;_tmp134=_tmp134->tl){Cyc_Absynpp_effects2docs(
rgions,effects,(void*)_tmp134->hd);}goto _LLB5;_LLBA:;_LLBB:*effects=({struct Cyc_List_List*
_tmp136=_cycalloc(sizeof(*_tmp136));_tmp136->hd=Cyc_Absynpp_typ2doc(t);_tmp136->tl=*
effects;_tmp136;});goto _LLB5;_LLB5:;}struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;Cyc_Absynpp_effects2docs(&
rgions,& effects,t);rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
rgions);effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
effects);if(rgions == 0  && effects != 0)return Cyc_PP_group(({const char*_tmp137="";
_tag_dyneither(_tmp137,sizeof(char),1);}),({const char*_tmp138="";_tag_dyneither(
_tmp138,sizeof(char),1);}),({const char*_tmp139="+";_tag_dyneither(_tmp139,
sizeof(char),2);}),effects);else{struct Cyc_PP_Doc*_tmp13A=Cyc_Absynpp_group_braces(({
const char*_tmp13F=",";_tag_dyneither(_tmp13F,sizeof(char),2);}),rgions);return
Cyc_PP_group(({const char*_tmp13B="";_tag_dyneither(_tmp13B,sizeof(char),1);}),({
const char*_tmp13C="";_tag_dyneither(_tmp13C,sizeof(char),1);}),({const char*
_tmp13D="+";_tag_dyneither(_tmp13D,sizeof(char),2);}),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,({
struct Cyc_List_List*_tmp13E=_cycalloc(sizeof(*_tmp13E));_tmp13E->hd=_tmp13A;
_tmp13E->tl=0;_tmp13E;})));}}struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind
k){switch(k){case Cyc_Absyn_StructA: _LLBC: return Cyc_PP_text(({const char*_tmp140="struct ";
_tag_dyneither(_tmp140,sizeof(char),8);}));case Cyc_Absyn_UnionA: _LLBD: return Cyc_PP_text(({
const char*_tmp141="union ";_tag_dyneither(_tmp141,sizeof(char),7);}));}}struct
Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){struct Cyc_PP_Doc*s;{void*_tmp142=t;
struct Cyc_Core_Opt*_tmp148;struct Cyc_Core_Opt*_tmp149;int _tmp14A;struct Cyc_Core_Opt*
_tmp14B;struct Cyc_Absyn_Tvar*_tmp14D;struct Cyc_Absyn_DatatypeInfo _tmp14F;union
Cyc_Absyn_DatatypeInfoU _tmp150;struct Cyc_List_List*_tmp151;struct Cyc_Absyn_DatatypeFieldInfo
_tmp153;union Cyc_Absyn_DatatypeFieldInfoU _tmp154;struct Cyc_List_List*_tmp155;
enum Cyc_Absyn_Sign _tmp157;enum Cyc_Absyn_Size_of _tmp158;int _tmp15B;struct Cyc_List_List*
_tmp15D;struct Cyc_Absyn_AggrInfo _tmp15F;union Cyc_Absyn_AggrInfoU _tmp160;struct
Cyc_List_List*_tmp161;enum Cyc_Absyn_AggrKind _tmp163;struct Cyc_List_List*_tmp164;
struct Cyc_List_List*_tmp166;struct _tuple0*_tmp168;struct Cyc_Absyn_Exp*_tmp16A;
struct _tuple0*_tmp16C;struct Cyc_List_List*_tmp16D;struct Cyc_Absyn_Typedefdecl*
_tmp16E;void*_tmp170;void*_tmp172;void*_tmp173;void*_tmp175;void*_tmp179;_LLC0: {
struct Cyc_Absyn_ArrayType_struct*_tmp143=(struct Cyc_Absyn_ArrayType_struct*)
_tmp142;if(_tmp143->tag != 9)goto _LLC2;}_LLC1: return Cyc_PP_text(({const char*
_tmp17C="[[[array]]]";_tag_dyneither(_tmp17C,sizeof(char),12);}));_LLC2: {struct
Cyc_Absyn_FnType_struct*_tmp144=(struct Cyc_Absyn_FnType_struct*)_tmp142;if(
_tmp144->tag != 10)goto _LLC4;}_LLC3: return Cyc_PP_nil_doc();_LLC4: {struct Cyc_Absyn_PointerType_struct*
_tmp145=(struct Cyc_Absyn_PointerType_struct*)_tmp142;if(_tmp145->tag != 5)goto
_LLC6;}_LLC5: return Cyc_PP_nil_doc();_LLC6: {struct Cyc_Absyn_VoidType_struct*
_tmp146=(struct Cyc_Absyn_VoidType_struct*)_tmp142;if(_tmp146->tag != 0)goto _LLC8;}
_LLC7: s=Cyc_PP_text(({const char*_tmp17D="void";_tag_dyneither(_tmp17D,sizeof(
char),5);}));goto _LLBF;_LLC8: {struct Cyc_Absyn_Evar_struct*_tmp147=(struct Cyc_Absyn_Evar_struct*)
_tmp142;if(_tmp147->tag != 1)goto _LLCA;else{_tmp148=_tmp147->f1;_tmp149=_tmp147->f2;
_tmp14A=_tmp147->f3;_tmp14B=_tmp147->f4;}}_LLC9: if(_tmp149 != 0)return Cyc_Absynpp_ntyp2doc((
void*)_tmp149->v);else{s=({struct Cyc_PP_Doc*_tmp17E[6];_tmp17E[5]=_tmp148 == 0?
Cyc_Absynpp_question(): Cyc_Absynpp_kind2doc((enum Cyc_Absyn_Kind)_tmp148->v);
_tmp17E[4]=Cyc_PP_text(({const char*_tmp185=")::";_tag_dyneither(_tmp185,sizeof(
char),4);}));_tmp17E[3]=(!Cyc_Absynpp_print_full_evars  || _tmp14B == 0)?Cyc_PP_nil_doc():
Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)_tmp14B->v);_tmp17E[2]=Cyc_PP_text((
struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp184;_tmp184.tag=1;_tmp184.f1=(
unsigned long)_tmp14A;({void*_tmp182[1]={& _tmp184};Cyc_aprintf(({const char*
_tmp183="%d";_tag_dyneither(_tmp183,sizeof(char),3);}),_tag_dyneither(_tmp182,
sizeof(void*),1));});}));_tmp17E[1]=Cyc_PP_text(({const char*_tmp181="(";
_tag_dyneither(_tmp181,sizeof(char),2);}));_tmp17E[0]=Cyc_Absynpp_print_for_cycdoc?
Cyc_PP_text(({const char*_tmp17F="\\%";_tag_dyneither(_tmp17F,sizeof(char),3);})):
Cyc_PP_text(({const char*_tmp180="%";_tag_dyneither(_tmp180,sizeof(char),2);}));
Cyc_PP_cat(_tag_dyneither(_tmp17E,sizeof(struct Cyc_PP_Doc*),6));});}goto _LLBF;
_LLCA: {struct Cyc_Absyn_VarType_struct*_tmp14C=(struct Cyc_Absyn_VarType_struct*)
_tmp142;if(_tmp14C->tag != 2)goto _LLCC;else{_tmp14D=_tmp14C->f1;}}_LLCB: s=Cyc_PP_textptr(
_tmp14D->name);if(Cyc_Absynpp_print_all_kinds)s=({struct Cyc_PP_Doc*_tmp186[3];
_tmp186[2]=Cyc_Absynpp_kindbound2doc(_tmp14D->kind);_tmp186[1]=Cyc_PP_text(({
const char*_tmp187="::";_tag_dyneither(_tmp187,sizeof(char),3);}));_tmp186[0]=s;
Cyc_PP_cat(_tag_dyneither(_tmp186,sizeof(struct Cyc_PP_Doc*),3));});if(Cyc_Absynpp_rewrite_temp_tvars
 && Cyc_Tcutil_is_temp_tvar(_tmp14D))s=({struct Cyc_PP_Doc*_tmp188[3];_tmp188[2]=
Cyc_PP_text(({const char*_tmp18A=" */";_tag_dyneither(_tmp18A,sizeof(char),4);}));
_tmp188[1]=s;_tmp188[0]=Cyc_PP_text(({const char*_tmp189="_ /* ";_tag_dyneither(
_tmp189,sizeof(char),6);}));Cyc_PP_cat(_tag_dyneither(_tmp188,sizeof(struct Cyc_PP_Doc*),
3));});goto _LLBF;_LLCC: {struct Cyc_Absyn_DatatypeType_struct*_tmp14E=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp142;if(_tmp14E->tag != 3)goto _LLCE;else{_tmp14F=_tmp14E->f1;_tmp150=_tmp14F.datatype_info;
_tmp151=_tmp14F.targs;}}_LLCD:{union Cyc_Absyn_DatatypeInfoU _tmp18B=_tmp150;
struct Cyc_Absyn_UnknownDatatypeInfo _tmp18C;struct _tuple0*_tmp18D;int _tmp18E;
struct Cyc_Absyn_Datatypedecl**_tmp18F;struct Cyc_Absyn_Datatypedecl*_tmp190;
struct Cyc_Absyn_Datatypedecl _tmp191;struct _tuple0*_tmp192;int _tmp193;_LLF5: if((
_tmp18B.UnknownDatatype).tag != 1)goto _LLF7;_tmp18C=(struct Cyc_Absyn_UnknownDatatypeInfo)(
_tmp18B.UnknownDatatype).val;_tmp18D=_tmp18C.name;_tmp18E=_tmp18C.is_extensible;
_LLF6: _tmp192=_tmp18D;_tmp193=_tmp18E;goto _LLF8;_LLF7: if((_tmp18B.KnownDatatype).tag
!= 2)goto _LLF4;_tmp18F=(struct Cyc_Absyn_Datatypedecl**)(_tmp18B.KnownDatatype).val;
_tmp190=*_tmp18F;_tmp191=*_tmp190;_tmp192=_tmp191.name;_tmp193=_tmp191.is_extensible;
_LLF8: {struct Cyc_PP_Doc*_tmp194=Cyc_PP_text(({const char*_tmp198="datatype ";
_tag_dyneither(_tmp198,sizeof(char),10);}));struct Cyc_PP_Doc*_tmp195=_tmp193?Cyc_PP_text(({
const char*_tmp197="@extensible ";_tag_dyneither(_tmp197,sizeof(char),13);})): Cyc_PP_nil_doc();
s=({struct Cyc_PP_Doc*_tmp196[4];_tmp196[3]=Cyc_Absynpp_tps2doc(_tmp151);_tmp196[
2]=Cyc_Absynpp_qvar2doc(_tmp192);_tmp196[1]=_tmp194;_tmp196[0]=_tmp195;Cyc_PP_cat(
_tag_dyneither(_tmp196,sizeof(struct Cyc_PP_Doc*),4));});goto _LLF4;}_LLF4:;}goto
_LLBF;_LLCE: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp152=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp142;if(_tmp152->tag != 4)goto _LLD0;else{_tmp153=_tmp152->f1;_tmp154=_tmp153.field_info;
_tmp155=_tmp153.targs;}}_LLCF:{union Cyc_Absyn_DatatypeFieldInfoU _tmp199=_tmp154;
struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp19A;struct _tuple0*_tmp19B;struct
_tuple0*_tmp19C;int _tmp19D;struct _tuple1 _tmp19E;struct Cyc_Absyn_Datatypedecl*
_tmp19F;struct Cyc_Absyn_Datatypedecl _tmp1A0;struct _tuple0*_tmp1A1;int _tmp1A2;
struct Cyc_Absyn_Datatypefield*_tmp1A3;struct Cyc_Absyn_Datatypefield _tmp1A4;
struct _tuple0*_tmp1A5;_LLFA: if((_tmp199.UnknownDatatypefield).tag != 1)goto _LLFC;
_tmp19A=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp199.UnknownDatatypefield).val;
_tmp19B=_tmp19A.datatype_name;_tmp19C=_tmp19A.field_name;_tmp19D=_tmp19A.is_extensible;
_LLFB: _tmp1A1=_tmp19B;_tmp1A2=_tmp19D;_tmp1A5=_tmp19C;goto _LLFD;_LLFC: if((
_tmp199.KnownDatatypefield).tag != 2)goto _LLF9;_tmp19E=(struct _tuple1)(_tmp199.KnownDatatypefield).val;
_tmp19F=_tmp19E.f1;_tmp1A0=*_tmp19F;_tmp1A1=_tmp1A0.name;_tmp1A2=_tmp1A0.is_extensible;
_tmp1A3=_tmp19E.f2;_tmp1A4=*_tmp1A3;_tmp1A5=_tmp1A4.name;_LLFD: {struct Cyc_PP_Doc*
_tmp1A6=Cyc_PP_text(_tmp1A2?({const char*_tmp1A9="@extensible datatype ";
_tag_dyneither(_tmp1A9,sizeof(char),22);}):({const char*_tmp1AA="datatype ";
_tag_dyneither(_tmp1AA,sizeof(char),10);}));s=({struct Cyc_PP_Doc*_tmp1A7[4];
_tmp1A7[3]=Cyc_Absynpp_qvar2doc(_tmp1A5);_tmp1A7[2]=Cyc_PP_text(({const char*
_tmp1A8=".";_tag_dyneither(_tmp1A8,sizeof(char),2);}));_tmp1A7[1]=Cyc_Absynpp_qvar2doc(
_tmp1A1);_tmp1A7[0]=_tmp1A6;Cyc_PP_cat(_tag_dyneither(_tmp1A7,sizeof(struct Cyc_PP_Doc*),
4));});goto _LLF9;}_LLF9:;}goto _LLBF;_LLD0: {struct Cyc_Absyn_IntType_struct*
_tmp156=(struct Cyc_Absyn_IntType_struct*)_tmp142;if(_tmp156->tag != 6)goto _LLD2;
else{_tmp157=_tmp156->f1;_tmp158=_tmp156->f2;}}_LLD1: {struct _dyneither_ptr sns;
struct _dyneither_ptr ts;switch(_tmp157){case Cyc_Absyn_None: _LLFE: goto _LLFF;case
Cyc_Absyn_Signed: _LLFF: sns=({const char*_tmp1AB="";_tag_dyneither(_tmp1AB,sizeof(
char),1);});break;case Cyc_Absyn_Unsigned: _LL100: sns=({const char*_tmp1AC="unsigned ";
_tag_dyneither(_tmp1AC,sizeof(char),10);});break;}switch(_tmp158){case Cyc_Absyn_Char_sz:
_LL102: switch(_tmp157){case Cyc_Absyn_None: _LL104: sns=({const char*_tmp1AD="";
_tag_dyneither(_tmp1AD,sizeof(char),1);});break;case Cyc_Absyn_Signed: _LL105: sns=({
const char*_tmp1AE="signed ";_tag_dyneither(_tmp1AE,sizeof(char),8);});break;case
Cyc_Absyn_Unsigned: _LL106: sns=({const char*_tmp1AF="unsigned ";_tag_dyneither(
_tmp1AF,sizeof(char),10);});break;}ts=({const char*_tmp1B0="char";_tag_dyneither(
_tmp1B0,sizeof(char),5);});break;case Cyc_Absyn_Short_sz: _LL103: ts=({const char*
_tmp1B1="short";_tag_dyneither(_tmp1B1,sizeof(char),6);});break;case Cyc_Absyn_Int_sz:
_LL108: ts=({const char*_tmp1B2="int";_tag_dyneither(_tmp1B2,sizeof(char),4);});
break;case Cyc_Absyn_Long_sz: _LL109: ts=({const char*_tmp1B3="long";_tag_dyneither(
_tmp1B3,sizeof(char),5);});break;case Cyc_Absyn_LongLong_sz: _LL10A: switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LL10C: ts=({const char*_tmp1B4="long long";_tag_dyneither(
_tmp1B4,sizeof(char),10);});break;case Cyc_Cyclone_Vc_c: _LL10D: ts=({const char*
_tmp1B5="__int64";_tag_dyneither(_tmp1B5,sizeof(char),8);});break;}break;}s=Cyc_PP_text((
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp1B9;_tmp1B9.tag=0;_tmp1B9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)ts);({struct Cyc_String_pa_struct
_tmp1B8;_tmp1B8.tag=0;_tmp1B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
sns);({void*_tmp1B6[2]={& _tmp1B8,& _tmp1B9};Cyc_aprintf(({const char*_tmp1B7="%s%s";
_tag_dyneither(_tmp1B7,sizeof(char),5);}),_tag_dyneither(_tmp1B6,sizeof(void*),2));});});}));
goto _LLBF;}_LLD2: {struct Cyc_Absyn_FloatType_struct*_tmp159=(struct Cyc_Absyn_FloatType_struct*)
_tmp142;if(_tmp159->tag != 7)goto _LLD4;}_LLD3: s=Cyc_PP_text(({const char*_tmp1BA="float";
_tag_dyneither(_tmp1BA,sizeof(char),6);}));goto _LLBF;_LLD4: {struct Cyc_Absyn_DoubleType_struct*
_tmp15A=(struct Cyc_Absyn_DoubleType_struct*)_tmp142;if(_tmp15A->tag != 8)goto
_LLD6;else{_tmp15B=_tmp15A->f1;}}_LLD5: if(_tmp15B)s=Cyc_PP_text(({const char*
_tmp1BB="long double";_tag_dyneither(_tmp1BB,sizeof(char),12);}));else{s=Cyc_PP_text(({
const char*_tmp1BC="double";_tag_dyneither(_tmp1BC,sizeof(char),7);}));}goto _LLBF;
_LLD6: {struct Cyc_Absyn_TupleType_struct*_tmp15C=(struct Cyc_Absyn_TupleType_struct*)
_tmp142;if(_tmp15C->tag != 11)goto _LLD8;else{_tmp15D=_tmp15C->f1;}}_LLD7: s=({
struct Cyc_PP_Doc*_tmp1BD[2];_tmp1BD[1]=Cyc_Absynpp_args2doc(_tmp15D);_tmp1BD[0]=
Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_dyneither(_tmp1BD,sizeof(struct Cyc_PP_Doc*),
2));});goto _LLBF;_LLD8: {struct Cyc_Absyn_AggrType_struct*_tmp15E=(struct Cyc_Absyn_AggrType_struct*)
_tmp142;if(_tmp15E->tag != 12)goto _LLDA;else{_tmp15F=_tmp15E->f1;_tmp160=_tmp15F.aggr_info;
_tmp161=_tmp15F.targs;}}_LLD9: {enum Cyc_Absyn_AggrKind _tmp1BF;struct _tuple0*
_tmp1C0;struct _tuple9 _tmp1BE=Cyc_Absyn_aggr_kinded_name(_tmp160);_tmp1BF=_tmp1BE.f1;
_tmp1C0=_tmp1BE.f2;s=({struct Cyc_PP_Doc*_tmp1C1[3];_tmp1C1[2]=Cyc_Absynpp_tps2doc(
_tmp161);_tmp1C1[1]=Cyc_Absynpp_qvar2doc(_tmp1C0);_tmp1C1[0]=Cyc_Absynpp_aggr_kind2doc(
_tmp1BF);Cyc_PP_cat(_tag_dyneither(_tmp1C1,sizeof(struct Cyc_PP_Doc*),3));});goto
_LLBF;}_LLDA: {struct Cyc_Absyn_AnonAggrType_struct*_tmp162=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp142;if(_tmp162->tag != 13)goto _LLDC;else{_tmp163=_tmp162->f1;_tmp164=_tmp162->f2;}}
_LLDB: s=({struct Cyc_PP_Doc*_tmp1C2[4];_tmp1C2[3]=Cyc_Absynpp_rb();_tmp1C2[2]=Cyc_PP_nest(
2,Cyc_Absynpp_aggrfields2doc(_tmp164));_tmp1C2[1]=Cyc_Absynpp_lb();_tmp1C2[0]=
Cyc_Absynpp_aggr_kind2doc(_tmp163);Cyc_PP_cat(_tag_dyneither(_tmp1C2,sizeof(
struct Cyc_PP_Doc*),4));});goto _LLBF;_LLDC: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp165=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp142;if(_tmp165->tag != 15)goto
_LLDE;else{_tmp166=_tmp165->f1;}}_LLDD: s=({struct Cyc_PP_Doc*_tmp1C3[4];_tmp1C3[3]=
Cyc_Absynpp_rb();_tmp1C3[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp166));
_tmp1C3[1]=Cyc_Absynpp_lb();_tmp1C3[0]=Cyc_PP_text(({const char*_tmp1C4="enum ";
_tag_dyneither(_tmp1C4,sizeof(char),6);}));Cyc_PP_cat(_tag_dyneither(_tmp1C3,
sizeof(struct Cyc_PP_Doc*),4));});goto _LLBF;_LLDE: {struct Cyc_Absyn_EnumType_struct*
_tmp167=(struct Cyc_Absyn_EnumType_struct*)_tmp142;if(_tmp167->tag != 14)goto _LLE0;
else{_tmp168=_tmp167->f1;}}_LLDF: s=({struct Cyc_PP_Doc*_tmp1C5[2];_tmp1C5[1]=Cyc_Absynpp_qvar2doc(
_tmp168);_tmp1C5[0]=Cyc_PP_text(({const char*_tmp1C6="enum ";_tag_dyneither(
_tmp1C6,sizeof(char),6);}));Cyc_PP_cat(_tag_dyneither(_tmp1C5,sizeof(struct Cyc_PP_Doc*),
2));});goto _LLBF;_LLE0: {struct Cyc_Absyn_ValueofType_struct*_tmp169=(struct Cyc_Absyn_ValueofType_struct*)
_tmp142;if(_tmp169->tag != 19)goto _LLE2;else{_tmp16A=_tmp169->f1;}}_LLE1: s=({
struct Cyc_PP_Doc*_tmp1C7[3];_tmp1C7[2]=Cyc_PP_text(({const char*_tmp1C9=")";
_tag_dyneither(_tmp1C9,sizeof(char),2);}));_tmp1C7[1]=Cyc_Absynpp_exp2doc(
_tmp16A);_tmp1C7[0]=Cyc_PP_text(({const char*_tmp1C8="valueof_t(";_tag_dyneither(
_tmp1C8,sizeof(char),11);}));Cyc_PP_cat(_tag_dyneither(_tmp1C7,sizeof(struct Cyc_PP_Doc*),
3));});goto _LLBF;_LLE2: {struct Cyc_Absyn_TypedefType_struct*_tmp16B=(struct Cyc_Absyn_TypedefType_struct*)
_tmp142;if(_tmp16B->tag != 18)goto _LLE4;else{_tmp16C=_tmp16B->f1;_tmp16D=_tmp16B->f2;
_tmp16E=_tmp16B->f3;}}_LLE3: s=({struct Cyc_PP_Doc*_tmp1CA[2];_tmp1CA[1]=Cyc_Absynpp_tps2doc(
_tmp16D);_tmp1CA[0]=Cyc_Absynpp_qvar2doc(_tmp16C);Cyc_PP_cat(_tag_dyneither(
_tmp1CA,sizeof(struct Cyc_PP_Doc*),2));});goto _LLBF;_LLE4: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp16F=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp142;if(_tmp16F->tag != 16)goto
_LLE6;else{_tmp170=(void*)_tmp16F->f1;}}_LLE5: s=({struct Cyc_PP_Doc*_tmp1CB[3];
_tmp1CB[2]=Cyc_PP_text(({const char*_tmp1CD=">";_tag_dyneither(_tmp1CD,sizeof(
char),2);}));_tmp1CB[1]=Cyc_Absynpp_rgn2doc(_tmp170);_tmp1CB[0]=Cyc_PP_text(({
const char*_tmp1CC="region_t<";_tag_dyneither(_tmp1CC,sizeof(char),10);}));Cyc_PP_cat(
_tag_dyneither(_tmp1CB,sizeof(struct Cyc_PP_Doc*),3));});goto _LLBF;_LLE6: {struct
Cyc_Absyn_DynRgnType_struct*_tmp171=(struct Cyc_Absyn_DynRgnType_struct*)_tmp142;
if(_tmp171->tag != 17)goto _LLE8;else{_tmp172=(void*)_tmp171->f1;_tmp173=(void*)
_tmp171->f2;}}_LLE7: s=({struct Cyc_PP_Doc*_tmp1CE[5];_tmp1CE[4]=Cyc_PP_text(({
const char*_tmp1D1=">";_tag_dyneither(_tmp1D1,sizeof(char),2);}));_tmp1CE[3]=Cyc_Absynpp_rgn2doc(
_tmp173);_tmp1CE[2]=Cyc_PP_text(({const char*_tmp1D0=",";_tag_dyneither(_tmp1D0,
sizeof(char),2);}));_tmp1CE[1]=Cyc_Absynpp_rgn2doc(_tmp172);_tmp1CE[0]=Cyc_PP_text(({
const char*_tmp1CF="dynregion_t<";_tag_dyneither(_tmp1CF,sizeof(char),13);}));Cyc_PP_cat(
_tag_dyneither(_tmp1CE,sizeof(struct Cyc_PP_Doc*),5));});goto _LLBF;_LLE8: {struct
Cyc_Absyn_TagType_struct*_tmp174=(struct Cyc_Absyn_TagType_struct*)_tmp142;if(
_tmp174->tag != 20)goto _LLEA;else{_tmp175=(void*)_tmp174->f1;}}_LLE9: s=({struct
Cyc_PP_Doc*_tmp1D2[3];_tmp1D2[2]=Cyc_PP_text(({const char*_tmp1D4=">";
_tag_dyneither(_tmp1D4,sizeof(char),2);}));_tmp1D2[1]=Cyc_Absynpp_typ2doc(
_tmp175);_tmp1D2[0]=Cyc_PP_text(({const char*_tmp1D3="tag_t<";_tag_dyneither(
_tmp1D3,sizeof(char),7);}));Cyc_PP_cat(_tag_dyneither(_tmp1D2,sizeof(struct Cyc_PP_Doc*),
3));});goto _LLBF;_LLEA: {struct Cyc_Absyn_UniqueRgn_struct*_tmp176=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp142;if(_tmp176->tag != 22)goto _LLEC;}_LLEB: goto _LLED;_LLEC: {struct Cyc_Absyn_HeapRgn_struct*
_tmp177=(struct Cyc_Absyn_HeapRgn_struct*)_tmp142;if(_tmp177->tag != 21)goto _LLEE;}
_LLED: s=Cyc_Absynpp_rgn2doc(t);goto _LLBF;_LLEE: {struct Cyc_Absyn_RgnsEff_struct*
_tmp178=(struct Cyc_Absyn_RgnsEff_struct*)_tmp142;if(_tmp178->tag != 25)goto _LLF0;
else{_tmp179=(void*)_tmp178->f1;}}_LLEF: s=({struct Cyc_PP_Doc*_tmp1D5[3];_tmp1D5[
2]=Cyc_PP_text(({const char*_tmp1D7=")";_tag_dyneither(_tmp1D7,sizeof(char),2);}));
_tmp1D5[1]=Cyc_Absynpp_typ2doc(_tmp179);_tmp1D5[0]=Cyc_PP_text(({const char*
_tmp1D6="regions(";_tag_dyneither(_tmp1D6,sizeof(char),9);}));Cyc_PP_cat(
_tag_dyneither(_tmp1D5,sizeof(struct Cyc_PP_Doc*),3));});goto _LLBF;_LLF0: {struct
Cyc_Absyn_AccessEff_struct*_tmp17A=(struct Cyc_Absyn_AccessEff_struct*)_tmp142;
if(_tmp17A->tag != 23)goto _LLF2;}_LLF1: goto _LLF3;_LLF2: {struct Cyc_Absyn_JoinEff_struct*
_tmp17B=(struct Cyc_Absyn_JoinEff_struct*)_tmp142;if(_tmp17B->tag != 24)goto _LLBF;}
_LLF3: s=Cyc_Absynpp_eff2doc(t);goto _LLBF;_LLBF:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(
struct Cyc_Core_Opt*vo){return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*((struct
_dyneither_ptr*)vo->v));}struct _tuple16{void*f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(
struct _tuple16*cmp){struct _tuple16 _tmp1D9;void*_tmp1DA;void*_tmp1DB;struct
_tuple16*_tmp1D8=cmp;_tmp1D9=*_tmp1D8;_tmp1DA=_tmp1D9.f1;_tmp1DB=_tmp1D9.f2;
return({struct Cyc_PP_Doc*_tmp1DC[3];_tmp1DC[2]=Cyc_Absynpp_rgn2doc(_tmp1DB);
_tmp1DC[1]=Cyc_PP_text(({const char*_tmp1DD=" > ";_tag_dyneither(_tmp1DD,sizeof(
char),4);}));_tmp1DC[0]=Cyc_Absynpp_rgn2doc(_tmp1DA);Cyc_PP_cat(_tag_dyneither(
_tmp1DC,sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(
struct Cyc_List_List*po){return Cyc_PP_group(({const char*_tmp1DE="";_tag_dyneither(
_tmp1DE,sizeof(char),1);}),({const char*_tmp1DF="";_tag_dyneither(_tmp1DF,sizeof(
char),1);}),({const char*_tmp1E0=",";_tag_dyneither(_tmp1E0,sizeof(char),2);}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(
struct _tuple7*t){struct Cyc_Core_Opt*dopt=(*t).f1 == 0?0:({struct Cyc_Core_Opt*
_tmp1E1=_cycalloc(sizeof(*_tmp1E1));_tmp1E1->v=Cyc_PP_text(*((struct
_dyneither_ptr*)((struct Cyc_Core_Opt*)_check_null((*t).f1))->v));_tmp1E1;});
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po){struct Cyc_List_List*
_tmp1E2=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple7*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);struct Cyc_PP_Doc*
eff_doc;if(c_varargs)_tmp1E2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(_tmp1E2,({struct Cyc_List_List*_tmp1E3=
_cycalloc(sizeof(*_tmp1E3));_tmp1E3->hd=Cyc_PP_text(({const char*_tmp1E4="...";
_tag_dyneither(_tmp1E4,sizeof(char),4);}));_tmp1E3->tl=0;_tmp1E3;}));else{if(
cyc_varargs != 0){struct Cyc_PP_Doc*_tmp1E5=({struct Cyc_PP_Doc*_tmp1E7[3];_tmp1E7[
2]=Cyc_Absynpp_funarg2doc(({struct _tuple7*_tmp1EB=_cycalloc(sizeof(*_tmp1EB));
_tmp1EB->f1=cyc_varargs->name;_tmp1EB->f2=cyc_varargs->tq;_tmp1EB->f3=
cyc_varargs->type;_tmp1EB;}));_tmp1E7[1]=cyc_varargs->inject?Cyc_PP_text(({const
char*_tmp1E9=" inject ";_tag_dyneither(_tmp1E9,sizeof(char),9);})): Cyc_PP_text(({
const char*_tmp1EA=" ";_tag_dyneither(_tmp1EA,sizeof(char),2);}));_tmp1E7[0]=Cyc_PP_text(({
const char*_tmp1E8="...";_tag_dyneither(_tmp1E8,sizeof(char),4);}));Cyc_PP_cat(
_tag_dyneither(_tmp1E7,sizeof(struct Cyc_PP_Doc*),3));});_tmp1E2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1E2,({struct
Cyc_List_List*_tmp1E6=_cycalloc(sizeof(*_tmp1E6));_tmp1E6->hd=_tmp1E5;_tmp1E6->tl=
0;_tmp1E6;}));}}{struct Cyc_PP_Doc*_tmp1EC=Cyc_PP_group(({const char*_tmp1F4="";
_tag_dyneither(_tmp1F4,sizeof(char),1);}),({const char*_tmp1F5="";_tag_dyneither(
_tmp1F5,sizeof(char),1);}),({const char*_tmp1F6=",";_tag_dyneither(_tmp1F6,
sizeof(char),2);}),_tmp1E2);if(effopt != 0  && Cyc_Absynpp_print_all_effects)
_tmp1EC=({struct Cyc_PP_Doc*_tmp1ED[3];_tmp1ED[2]=Cyc_Absynpp_eff2doc((void*)
effopt->v);_tmp1ED[1]=Cyc_PP_text(({const char*_tmp1EE=";";_tag_dyneither(_tmp1EE,
sizeof(char),2);}));_tmp1ED[0]=_tmp1EC;Cyc_PP_cat(_tag_dyneither(_tmp1ED,sizeof(
struct Cyc_PP_Doc*),3));});if(rgn_po != 0)_tmp1EC=({struct Cyc_PP_Doc*_tmp1EF[3];
_tmp1EF[2]=Cyc_Absynpp_rgnpo2doc(rgn_po);_tmp1EF[1]=Cyc_PP_text(({const char*
_tmp1F0=":";_tag_dyneither(_tmp1F0,sizeof(char),2);}));_tmp1EF[0]=_tmp1EC;Cyc_PP_cat(
_tag_dyneither(_tmp1EF,sizeof(struct Cyc_PP_Doc*),3));});return({struct Cyc_PP_Doc*
_tmp1F1[3];_tmp1F1[2]=Cyc_PP_text(({const char*_tmp1F3=")";_tag_dyneither(_tmp1F3,
sizeof(char),2);}));_tmp1F1[1]=_tmp1EC;_tmp1F1[0]=Cyc_PP_text(({const char*
_tmp1F2="(";_tag_dyneither(_tmp1F2,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(
_tmp1F1,sizeof(struct Cyc_PP_Doc*),3));});};}struct _tuple7*Cyc_Absynpp_arg_mk_opt(
struct _tuple11*arg){return({struct _tuple7*_tmp1F7=_cycalloc(sizeof(*_tmp1F7));
_tmp1F7->f1=({struct Cyc_Core_Opt*_tmp1F8=_cycalloc(sizeof(*_tmp1F8));_tmp1F8->v=(*
arg).f1;_tmp1F8;});_tmp1F7->f2=(*arg).f2;_tmp1F7->f3=(*arg).f3;_tmp1F7;});}
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dyneither_ptr*v){return Cyc_PP_text(*
v);}struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){struct Cyc_List_List*
_tmp1F9=0;int match;{union Cyc_Absyn_Nmspace _tmp1FA=(*q).f1;int _tmp1FB;struct Cyc_List_List*
_tmp1FC;struct Cyc_List_List*_tmp1FD;_LL110: if((_tmp1FA.Loc_n).tag != 3)goto _LL112;
_tmp1FB=(int)(_tmp1FA.Loc_n).val;_LL111: _tmp1FC=0;goto _LL113;_LL112: if((_tmp1FA.Rel_n).tag
!= 1)goto _LL114;_tmp1FC=(struct Cyc_List_List*)(_tmp1FA.Rel_n).val;_LL113: match=0;
_tmp1F9=_tmp1FC;goto _LL10F;_LL114: if((_tmp1FA.Abs_n).tag != 2)goto _LL10F;_tmp1FD=(
struct Cyc_List_List*)(_tmp1FA.Abs_n).val;_LL115: match=Cyc_Absynpp_use_curr_namespace
 && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1FD,Cyc_Absynpp_curr_namespace);
_tmp1F9=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?({struct Cyc_List_List*
_tmp1FE=_cycalloc(sizeof(*_tmp1FE));_tmp1FE->hd=Cyc_Absynpp_cyc_stringptr;
_tmp1FE->tl=_tmp1FD;_tmp1FE;}): _tmp1FD;goto _LL10F;_LL10F:;}if(Cyc_Absynpp_qvar_to_Cids)
return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1F9,({struct Cyc_List_List*_tmp1FF=
_cycalloc(sizeof(*_tmp1FF));_tmp1FF->hd=(*q).f2;_tmp1FF->tl=0;_tmp1FF;})),({
const char*_tmp200="_";_tag_dyneither(_tmp200,sizeof(char),2);}));else{if(match)
return*(*q).f2;else{return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1F9,({struct
Cyc_List_List*_tmp201=_cycalloc(sizeof(*_tmp201));_tmp201->hd=(*q).f2;_tmp201->tl=
0;_tmp201;})),({const char*_tmp202="::";_tag_dyneither(_tmp202,sizeof(char),3);}));}}}
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){return Cyc_PP_text(Cyc_Absynpp_qvar2string(
q));}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){struct
_dyneither_ptr _tmp203=Cyc_Absynpp_qvar2string(q);if(Cyc_PP_tex_output)return Cyc_PP_text_width((
struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(({const
char*_tmp204="\\textbf{";_tag_dyneither(_tmp204,sizeof(char),9);}),(struct
_dyneither_ptr)_tmp203),({const char*_tmp205="}";_tag_dyneither(_tmp205,sizeof(
char),2);})),(int)Cyc_strlen((struct _dyneither_ptr)_tmp203));else{return Cyc_PP_text(
_tmp203);}}struct _dyneither_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){
if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);if(Cyc_Absynpp_use_curr_namespace){
union Cyc_Absyn_Nmspace _tmp206=(*v).f1;int _tmp207;struct Cyc_List_List*_tmp208;
struct Cyc_List_List*_tmp209;_LL117: if((_tmp206.Loc_n).tag != 3)goto _LL119;_tmp207=(
int)(_tmp206.Loc_n).val;_LL118: goto _LL11A;_LL119: if((_tmp206.Rel_n).tag != 1)goto
_LL11B;_tmp208=(struct Cyc_List_List*)(_tmp206.Rel_n).val;if(_tmp208 != 0)goto
_LL11B;_LL11A: return*(*v).f2;_LL11B: if((_tmp206.Abs_n).tag != 2)goto _LL11D;
_tmp209=(struct Cyc_List_List*)(_tmp206.Abs_n).val;_LL11C: if(((int(*)(int(*cmp)(
struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_strptrcmp,_tmp209,Cyc_Absynpp_curr_namespace)== 0)
return*(*v).f2;else{goto _LL11E;}_LL11D:;_LL11E: return(struct _dyneither_ptr)Cyc_strconcat(({
const char*_tmp20A="/* bad namespace : */ ";_tag_dyneither(_tmp20A,sizeof(char),
23);}),(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(v));_LL116:;}else{return*(*
v).f2;}}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){return Cyc_PP_text(
Cyc_Absynpp_typedef_name2string(v));}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(
struct _tuple0*v){struct _dyneither_ptr _tmp20B=Cyc_Absynpp_typedef_name2string(v);
if(Cyc_PP_tex_output)return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((
struct _dyneither_ptr)Cyc_strconcat(({const char*_tmp20C="\\textbf{";
_tag_dyneither(_tmp20C,sizeof(char),9);}),(struct _dyneither_ptr)_tmp20B),({const
char*_tmp20D="}";_tag_dyneither(_tmp20D,sizeof(char),2);})),(int)Cyc_strlen((
struct _dyneither_ptr)_tmp20B));else{return Cyc_PP_text(_tmp20B);}}struct Cyc_PP_Doc*
Cyc_Absynpp_typ2doc(void*t){return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),
t,0);}int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){void*_tmp20E=e->r;enum Cyc_Absyn_Primop
_tmp213;struct Cyc_Absyn_Exp*_tmp21E;struct Cyc_Absyn_Exp*_tmp220;_LL120: {struct
Cyc_Absyn_Const_e_struct*_tmp20F=(struct Cyc_Absyn_Const_e_struct*)_tmp20E;if(
_tmp20F->tag != 0)goto _LL122;}_LL121: goto _LL123;_LL122: {struct Cyc_Absyn_Var_e_struct*
_tmp210=(struct Cyc_Absyn_Var_e_struct*)_tmp20E;if(_tmp210->tag != 1)goto _LL124;}
_LL123: goto _LL125;_LL124: {struct Cyc_Absyn_UnknownId_e_struct*_tmp211=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmp20E;if(_tmp211->tag != 2)goto _LL126;}_LL125: return 10000;_LL126: {struct Cyc_Absyn_Primop_e_struct*
_tmp212=(struct Cyc_Absyn_Primop_e_struct*)_tmp20E;if(_tmp212->tag != 3)goto _LL128;
else{_tmp213=_tmp212->f1;}}_LL127: switch(_tmp213){case Cyc_Absyn_Plus: _LL170:
return 100;case Cyc_Absyn_Times: _LL171: return 110;case Cyc_Absyn_Minus: _LL172: return
100;case Cyc_Absyn_Div: _LL173: goto _LL174;case Cyc_Absyn_Mod: _LL174: return 110;case
Cyc_Absyn_Eq: _LL175: goto _LL176;case Cyc_Absyn_Neq: _LL176: return 70;case Cyc_Absyn_Gt:
_LL177: goto _LL178;case Cyc_Absyn_Lt: _LL178: goto _LL179;case Cyc_Absyn_Gte: _LL179:
goto _LL17A;case Cyc_Absyn_Lte: _LL17A: return 80;case Cyc_Absyn_Not: _LL17B: goto _LL17C;
case Cyc_Absyn_Bitnot: _LL17C: return 130;case Cyc_Absyn_Bitand: _LL17D: return 60;case
Cyc_Absyn_Bitor: _LL17E: return 40;case Cyc_Absyn_Bitxor: _LL17F: return 50;case Cyc_Absyn_Bitlshift:
_LL180: return 90;case Cyc_Absyn_Bitlrshift: _LL181: return 80;case Cyc_Absyn_Bitarshift:
_LL182: return 80;case Cyc_Absyn_Numelts: _LL183: return 140;}_LL128: {struct Cyc_Absyn_AssignOp_e_struct*
_tmp214=(struct Cyc_Absyn_AssignOp_e_struct*)_tmp20E;if(_tmp214->tag != 4)goto
_LL12A;}_LL129: return 20;_LL12A: {struct Cyc_Absyn_Increment_e_struct*_tmp215=(
struct Cyc_Absyn_Increment_e_struct*)_tmp20E;if(_tmp215->tag != 5)goto _LL12C;}
_LL12B: return 130;_LL12C: {struct Cyc_Absyn_Conditional_e_struct*_tmp216=(struct
Cyc_Absyn_Conditional_e_struct*)_tmp20E;if(_tmp216->tag != 6)goto _LL12E;}_LL12D:
return 30;_LL12E: {struct Cyc_Absyn_And_e_struct*_tmp217=(struct Cyc_Absyn_And_e_struct*)
_tmp20E;if(_tmp217->tag != 7)goto _LL130;}_LL12F: return 35;_LL130: {struct Cyc_Absyn_Or_e_struct*
_tmp218=(struct Cyc_Absyn_Or_e_struct*)_tmp20E;if(_tmp218->tag != 8)goto _LL132;}
_LL131: return 30;_LL132: {struct Cyc_Absyn_SeqExp_e_struct*_tmp219=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp20E;if(_tmp219->tag != 9)goto _LL134;}_LL133: return 10;_LL134: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp21A=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp20E;if(_tmp21A->tag != 10)goto
_LL136;}_LL135: goto _LL137;_LL136: {struct Cyc_Absyn_FnCall_e_struct*_tmp21B=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp20E;if(_tmp21B->tag != 11)goto _LL138;}_LL137:
return 140;_LL138: {struct Cyc_Absyn_Throw_e_struct*_tmp21C=(struct Cyc_Absyn_Throw_e_struct*)
_tmp20E;if(_tmp21C->tag != 12)goto _LL13A;}_LL139: return 130;_LL13A: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp21D=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp20E;if(_tmp21D->tag != 13)
goto _LL13C;else{_tmp21E=_tmp21D->f1;}}_LL13B: return Cyc_Absynpp_exp_prec(_tmp21E);
_LL13C: {struct Cyc_Absyn_Instantiate_e_struct*_tmp21F=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp20E;if(_tmp21F->tag != 14)goto _LL13E;else{_tmp220=_tmp21F->f1;}}_LL13D: return
Cyc_Absynpp_exp_prec(_tmp220);_LL13E: {struct Cyc_Absyn_Cast_e_struct*_tmp221=(
struct Cyc_Absyn_Cast_e_struct*)_tmp20E;if(_tmp221->tag != 15)goto _LL140;}_LL13F:
return 120;_LL140: {struct Cyc_Absyn_New_e_struct*_tmp222=(struct Cyc_Absyn_New_e_struct*)
_tmp20E;if(_tmp222->tag != 17)goto _LL142;}_LL141: return 15;_LL142: {struct Cyc_Absyn_Address_e_struct*
_tmp223=(struct Cyc_Absyn_Address_e_struct*)_tmp20E;if(_tmp223->tag != 16)goto
_LL144;}_LL143: goto _LL145;_LL144: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp224=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp20E;if(_tmp224->tag != 18)goto _LL146;}
_LL145: goto _LL147;_LL146: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp225=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp20E;if(_tmp225->tag != 19)goto _LL148;}_LL147: goto _LL149;_LL148: {struct Cyc_Absyn_Valueof_e_struct*
_tmp226=(struct Cyc_Absyn_Valueof_e_struct*)_tmp20E;if(_tmp226->tag != 39)goto
_LL14A;}_LL149: goto _LL14B;_LL14A: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp227=(
struct Cyc_Absyn_Tagcheck_e_struct*)_tmp20E;if(_tmp227->tag != 38)goto _LL14C;}
_LL14B: goto _LL14D;_LL14C: {struct Cyc_Absyn_Offsetof_e_struct*_tmp228=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp20E;if(_tmp228->tag != 20)goto _LL14E;}_LL14D: goto _LL14F;_LL14E: {struct Cyc_Absyn_Deref_e_struct*
_tmp229=(struct Cyc_Absyn_Deref_e_struct*)_tmp20E;if(_tmp229->tag != 21)goto _LL150;}
_LL14F: return 130;_LL150: {struct Cyc_Absyn_AggrMember_e_struct*_tmp22A=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp20E;if(_tmp22A->tag != 22)goto _LL152;}_LL151: goto _LL153;_LL152: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp22B=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp20E;if(_tmp22B->tag != 23)goto
_LL154;}_LL153: goto _LL155;_LL154: {struct Cyc_Absyn_Subscript_e_struct*_tmp22C=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp20E;if(_tmp22C->tag != 24)goto _LL156;}
_LL155: return 140;_LL156: {struct Cyc_Absyn_Tuple_e_struct*_tmp22D=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp20E;if(_tmp22D->tag != 25)goto _LL158;}_LL157: return 150;_LL158: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp22E=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp20E;if(_tmp22E->tag != 26)goto
_LL15A;}_LL159: goto _LL15B;_LL15A: {struct Cyc_Absyn_Array_e_struct*_tmp22F=(
struct Cyc_Absyn_Array_e_struct*)_tmp20E;if(_tmp22F->tag != 27)goto _LL15C;}_LL15B:
goto _LL15D;_LL15C: {struct Cyc_Absyn_Comprehension_e_struct*_tmp230=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp20E;if(_tmp230->tag != 28)goto _LL15E;}_LL15D: goto _LL15F;_LL15E: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp231=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp20E;if(_tmp231->tag != 29)goto
_LL160;}_LL15F: goto _LL161;_LL160: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp232=(
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp20E;if(_tmp232->tag != 30)goto _LL162;}
_LL161: goto _LL163;_LL162: {struct Cyc_Absyn_Datatype_e_struct*_tmp233=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp20E;if(_tmp233->tag != 31)goto _LL164;}_LL163: goto _LL165;_LL164: {struct Cyc_Absyn_Enum_e_struct*
_tmp234=(struct Cyc_Absyn_Enum_e_struct*)_tmp20E;if(_tmp234->tag != 32)goto _LL166;}
_LL165: goto _LL167;_LL166: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp235=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp20E;if(_tmp235->tag != 33)goto _LL168;}_LL167: goto _LL169;_LL168: {struct Cyc_Absyn_Malloc_e_struct*
_tmp236=(struct Cyc_Absyn_Malloc_e_struct*)_tmp20E;if(_tmp236->tag != 34)goto
_LL16A;}_LL169: goto _LL16B;_LL16A: {struct Cyc_Absyn_Swap_e_struct*_tmp237=(struct
Cyc_Absyn_Swap_e_struct*)_tmp20E;if(_tmp237->tag != 35)goto _LL16C;}_LL16B: goto
_LL16D;_LL16C: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp238=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp20E;if(_tmp238->tag != 36)goto _LL16E;}_LL16D: return 140;_LL16E: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp239=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp20E;if(_tmp239->tag != 37)goto
_LL11F;}_LL16F: return 10000;_LL11F:;}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct
Cyc_Absyn_Exp*e){return Cyc_Absynpp_exp2doc_prec(0,e);}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*
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
unsigned int _tmp27E;struct Cyc_Absyn_Exp*_tmp280;struct Cyc_Absyn_Exp*_tmp282;
struct _dyneither_ptr*_tmp283;struct Cyc_Absyn_Exp*_tmp285;struct _dyneither_ptr*
_tmp286;struct Cyc_Absyn_Exp*_tmp288;struct Cyc_Absyn_Exp*_tmp289;struct Cyc_List_List*
_tmp28B;struct _tuple7*_tmp28D;struct Cyc_List_List*_tmp28E;struct Cyc_List_List*
_tmp290;struct Cyc_Absyn_Vardecl*_tmp292;struct Cyc_Absyn_Exp*_tmp293;struct Cyc_Absyn_Exp*
_tmp294;struct _tuple0*_tmp296;struct Cyc_List_List*_tmp297;struct Cyc_List_List*
_tmp298;struct Cyc_List_List*_tmp29A;struct Cyc_List_List*_tmp29C;struct Cyc_Absyn_Datatypefield*
_tmp29D;struct _tuple0*_tmp29F;struct _tuple0*_tmp2A1;struct Cyc_Absyn_MallocInfo
_tmp2A3;int _tmp2A4;struct Cyc_Absyn_Exp*_tmp2A5;void**_tmp2A6;struct Cyc_Absyn_Exp*
_tmp2A7;struct Cyc_Absyn_Exp*_tmp2A9;struct Cyc_Absyn_Exp*_tmp2AA;struct Cyc_Core_Opt*
_tmp2AC;struct Cyc_List_List*_tmp2AD;struct Cyc_Absyn_Stmt*_tmp2AF;_LL186: {struct
Cyc_Absyn_Const_e_struct*_tmp23B=(struct Cyc_Absyn_Const_e_struct*)_tmp23A;if(
_tmp23B->tag != 0)goto _LL188;else{_tmp23C=_tmp23B->f1;}}_LL187: s=Cyc_Absynpp_cnst2doc(
_tmp23C);goto _LL185;_LL188: {struct Cyc_Absyn_Var_e_struct*_tmp23D=(struct Cyc_Absyn_Var_e_struct*)
_tmp23A;if(_tmp23D->tag != 1)goto _LL18A;else{_tmp23E=_tmp23D->f1;}}_LL189: _tmp240=
_tmp23E;goto _LL18B;_LL18A: {struct Cyc_Absyn_UnknownId_e_struct*_tmp23F=(struct
Cyc_Absyn_UnknownId_e_struct*)_tmp23A;if(_tmp23F->tag != 2)goto _LL18C;else{
_tmp240=_tmp23F->f1;}}_LL18B: s=Cyc_Absynpp_qvar2doc(_tmp240);goto _LL185;_LL18C: {
struct Cyc_Absyn_Primop_e_struct*_tmp241=(struct Cyc_Absyn_Primop_e_struct*)
_tmp23A;if(_tmp241->tag != 3)goto _LL18E;else{_tmp242=_tmp241->f1;_tmp243=_tmp241->f2;}}
_LL18D: s=Cyc_Absynpp_primapp2doc(myprec,_tmp242,_tmp243);goto _LL185;_LL18E: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp244=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp23A;if(_tmp244->tag != 4)goto _LL190;else{_tmp245=_tmp244->f1;_tmp246=_tmp244->f2;
_tmp247=_tmp244->f3;}}_LL18F: s=({struct Cyc_PP_Doc*_tmp2B0[5];_tmp2B0[4]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp247);_tmp2B0[3]=Cyc_PP_text(({const char*_tmp2B2="= ";_tag_dyneither(
_tmp2B2,sizeof(char),3);}));_tmp2B0[2]=_tmp246 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((
enum Cyc_Absyn_Primop)_tmp246->v);_tmp2B0[1]=Cyc_PP_text(({const char*_tmp2B1=" ";
_tag_dyneither(_tmp2B1,sizeof(char),2);}));_tmp2B0[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp245);Cyc_PP_cat(_tag_dyneither(_tmp2B0,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL185;_LL190: {struct Cyc_Absyn_Increment_e_struct*_tmp248=(struct Cyc_Absyn_Increment_e_struct*)
_tmp23A;if(_tmp248->tag != 5)goto _LL192;else{_tmp249=_tmp248->f1;_tmp24A=_tmp248->f2;}}
_LL191: {struct Cyc_PP_Doc*_tmp2B3=Cyc_Absynpp_exp2doc_prec(myprec,_tmp249);
switch(_tmp24A){case Cyc_Absyn_PreInc: _LL1D8: s=({struct Cyc_PP_Doc*_tmp2B4[2];
_tmp2B4[1]=_tmp2B3;_tmp2B4[0]=Cyc_PP_text(({const char*_tmp2B5="++";
_tag_dyneither(_tmp2B5,sizeof(char),3);}));Cyc_PP_cat(_tag_dyneither(_tmp2B4,
sizeof(struct Cyc_PP_Doc*),2));});break;case Cyc_Absyn_PreDec: _LL1D9: s=({struct Cyc_PP_Doc*
_tmp2B6[2];_tmp2B6[1]=_tmp2B3;_tmp2B6[0]=Cyc_PP_text(({const char*_tmp2B7="--";
_tag_dyneither(_tmp2B7,sizeof(char),3);}));Cyc_PP_cat(_tag_dyneither(_tmp2B6,
sizeof(struct Cyc_PP_Doc*),2));});break;case Cyc_Absyn_PostInc: _LL1DA: s=({struct
Cyc_PP_Doc*_tmp2B8[2];_tmp2B8[1]=Cyc_PP_text(({const char*_tmp2B9="++";
_tag_dyneither(_tmp2B9,sizeof(char),3);}));_tmp2B8[0]=_tmp2B3;Cyc_PP_cat(
_tag_dyneither(_tmp2B8,sizeof(struct Cyc_PP_Doc*),2));});break;case Cyc_Absyn_PostDec:
_LL1DB: s=({struct Cyc_PP_Doc*_tmp2BA[2];_tmp2BA[1]=Cyc_PP_text(({const char*
_tmp2BB="--";_tag_dyneither(_tmp2BB,sizeof(char),3);}));_tmp2BA[0]=_tmp2B3;Cyc_PP_cat(
_tag_dyneither(_tmp2BA,sizeof(struct Cyc_PP_Doc*),2));});break;}goto _LL185;}
_LL192: {struct Cyc_Absyn_Conditional_e_struct*_tmp24B=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp23A;if(_tmp24B->tag != 6)goto _LL194;else{_tmp24C=_tmp24B->f1;_tmp24D=_tmp24B->f2;
_tmp24E=_tmp24B->f3;}}_LL193: s=({struct Cyc_PP_Doc*_tmp2BC[5];_tmp2BC[4]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp24E);_tmp2BC[3]=Cyc_PP_text(({const char*_tmp2BE=" : ";_tag_dyneither(
_tmp2BE,sizeof(char),4);}));_tmp2BC[2]=Cyc_Absynpp_exp2doc_prec(0,_tmp24D);
_tmp2BC[1]=Cyc_PP_text(({const char*_tmp2BD=" ? ";_tag_dyneither(_tmp2BD,sizeof(
char),4);}));_tmp2BC[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp24C);Cyc_PP_cat(
_tag_dyneither(_tmp2BC,sizeof(struct Cyc_PP_Doc*),5));});goto _LL185;_LL194: {
struct Cyc_Absyn_And_e_struct*_tmp24F=(struct Cyc_Absyn_And_e_struct*)_tmp23A;if(
_tmp24F->tag != 7)goto _LL196;else{_tmp250=_tmp24F->f1;_tmp251=_tmp24F->f2;}}
_LL195: s=({struct Cyc_PP_Doc*_tmp2BF[3];_tmp2BF[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp251);_tmp2BF[1]=Cyc_PP_text(({const char*_tmp2C0=" && ";_tag_dyneither(
_tmp2C0,sizeof(char),5);}));_tmp2BF[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp250);
Cyc_PP_cat(_tag_dyneither(_tmp2BF,sizeof(struct Cyc_PP_Doc*),3));});goto _LL185;
_LL196: {struct Cyc_Absyn_Or_e_struct*_tmp252=(struct Cyc_Absyn_Or_e_struct*)
_tmp23A;if(_tmp252->tag != 8)goto _LL198;else{_tmp253=_tmp252->f1;_tmp254=_tmp252->f2;}}
_LL197: s=({struct Cyc_PP_Doc*_tmp2C1[3];_tmp2C1[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp254);_tmp2C1[1]=Cyc_PP_text(({const char*_tmp2C2=" || ";_tag_dyneither(
_tmp2C2,sizeof(char),5);}));_tmp2C1[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp253);
Cyc_PP_cat(_tag_dyneither(_tmp2C1,sizeof(struct Cyc_PP_Doc*),3));});goto _LL185;
_LL198: {struct Cyc_Absyn_SeqExp_e_struct*_tmp255=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp23A;if(_tmp255->tag != 9)goto _LL19A;else{_tmp256=_tmp255->f1;_tmp257=_tmp255->f2;}}
_LL199: s=({struct Cyc_PP_Doc*_tmp2C3[5];_tmp2C3[4]=Cyc_PP_text(({const char*
_tmp2C6=")";_tag_dyneither(_tmp2C6,sizeof(char),2);}));_tmp2C3[3]=Cyc_Absynpp_exp2doc(
_tmp257);_tmp2C3[2]=Cyc_PP_text(({const char*_tmp2C5=", ";_tag_dyneither(_tmp2C5,
sizeof(char),3);}));_tmp2C3[1]=Cyc_Absynpp_exp2doc(_tmp256);_tmp2C3[0]=Cyc_PP_text(({
const char*_tmp2C4="(";_tag_dyneither(_tmp2C4,sizeof(char),2);}));Cyc_PP_cat(
_tag_dyneither(_tmp2C3,sizeof(struct Cyc_PP_Doc*),5));});goto _LL185;_LL19A: {
struct Cyc_Absyn_UnknownCall_e_struct*_tmp258=(struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp23A;if(_tmp258->tag != 10)goto _LL19C;else{_tmp259=_tmp258->f1;_tmp25A=_tmp258->f2;}}
_LL19B: s=({struct Cyc_PP_Doc*_tmp2C7[4];_tmp2C7[3]=Cyc_PP_text(({const char*
_tmp2C9=")";_tag_dyneither(_tmp2C9,sizeof(char),2);}));_tmp2C7[2]=Cyc_Absynpp_exps2doc_prec(
20,_tmp25A);_tmp2C7[1]=Cyc_PP_text(({const char*_tmp2C8="(";_tag_dyneither(
_tmp2C8,sizeof(char),2);}));_tmp2C7[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp259);
Cyc_PP_cat(_tag_dyneither(_tmp2C7,sizeof(struct Cyc_PP_Doc*),4));});goto _LL185;
_LL19C: {struct Cyc_Absyn_FnCall_e_struct*_tmp25B=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp23A;if(_tmp25B->tag != 11)goto _LL19E;else{_tmp25C=_tmp25B->f1;_tmp25D=_tmp25B->f2;}}
_LL19D: s=({struct Cyc_PP_Doc*_tmp2CA[4];_tmp2CA[3]=Cyc_PP_text(({const char*
_tmp2CC=")";_tag_dyneither(_tmp2CC,sizeof(char),2);}));_tmp2CA[2]=Cyc_Absynpp_exps2doc_prec(
20,_tmp25D);_tmp2CA[1]=Cyc_PP_text(({const char*_tmp2CB="(";_tag_dyneither(
_tmp2CB,sizeof(char),2);}));_tmp2CA[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp25C);
Cyc_PP_cat(_tag_dyneither(_tmp2CA,sizeof(struct Cyc_PP_Doc*),4));});goto _LL185;
_LL19E: {struct Cyc_Absyn_Throw_e_struct*_tmp25E=(struct Cyc_Absyn_Throw_e_struct*)
_tmp23A;if(_tmp25E->tag != 12)goto _LL1A0;else{_tmp25F=_tmp25E->f1;}}_LL19F: s=({
struct Cyc_PP_Doc*_tmp2CD[2];_tmp2CD[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp25F);
_tmp2CD[0]=Cyc_PP_text(({const char*_tmp2CE="throw ";_tag_dyneither(_tmp2CE,
sizeof(char),7);}));Cyc_PP_cat(_tag_dyneither(_tmp2CD,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL185;_LL1A0: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp260=(
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp23A;if(_tmp260->tag != 13)goto _LL1A2;
else{_tmp261=_tmp260->f1;}}_LL1A1: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp261);
goto _LL185;_LL1A2: {struct Cyc_Absyn_Instantiate_e_struct*_tmp262=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp23A;if(_tmp262->tag != 14)goto _LL1A4;else{_tmp263=_tmp262->f1;}}_LL1A3: s=Cyc_Absynpp_exp2doc_prec(
inprec,_tmp263);goto _LL185;_LL1A4: {struct Cyc_Absyn_Cast_e_struct*_tmp264=(
struct Cyc_Absyn_Cast_e_struct*)_tmp23A;if(_tmp264->tag != 15)goto _LL1A6;else{
_tmp265=(void*)_tmp264->f1;_tmp266=_tmp264->f2;}}_LL1A5: s=({struct Cyc_PP_Doc*
_tmp2CF[4];_tmp2CF[3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp266);_tmp2CF[2]=Cyc_PP_text(({
const char*_tmp2D1=")";_tag_dyneither(_tmp2D1,sizeof(char),2);}));_tmp2CF[1]=Cyc_Absynpp_typ2doc(
_tmp265);_tmp2CF[0]=Cyc_PP_text(({const char*_tmp2D0="(";_tag_dyneither(_tmp2D0,
sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp2CF,sizeof(struct Cyc_PP_Doc*),
4));});goto _LL185;_LL1A6: {struct Cyc_Absyn_Address_e_struct*_tmp267=(struct Cyc_Absyn_Address_e_struct*)
_tmp23A;if(_tmp267->tag != 16)goto _LL1A8;else{_tmp268=_tmp267->f1;}}_LL1A7: s=({
struct Cyc_PP_Doc*_tmp2D2[2];_tmp2D2[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp268);
_tmp2D2[0]=Cyc_PP_text(({const char*_tmp2D3="&";_tag_dyneither(_tmp2D3,sizeof(
char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp2D2,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL185;_LL1A8: {struct Cyc_Absyn_New_e_struct*_tmp269=(struct Cyc_Absyn_New_e_struct*)
_tmp23A;if(_tmp269->tag != 17)goto _LL1AA;else{_tmp26A=_tmp269->f1;_tmp26B=_tmp269->f2;}}
_LL1A9: if(_tmp26A == 0)s=({struct Cyc_PP_Doc*_tmp2D4[2];_tmp2D4[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp26B);_tmp2D4[0]=Cyc_PP_text(({const char*_tmp2D5="new ";_tag_dyneither(
_tmp2D5,sizeof(char),5);}));Cyc_PP_cat(_tag_dyneither(_tmp2D4,sizeof(struct Cyc_PP_Doc*),
2));});else{s=({struct Cyc_PP_Doc*_tmp2D6[4];_tmp2D6[3]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp26B);_tmp2D6[2]=Cyc_PP_text(({const char*_tmp2D8=") ";_tag_dyneither(
_tmp2D8,sizeof(char),3);}));_tmp2D6[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_tmp26A);_tmp2D6[0]=Cyc_PP_text(({const char*_tmp2D7="rnew(";_tag_dyneither(
_tmp2D7,sizeof(char),6);}));Cyc_PP_cat(_tag_dyneither(_tmp2D6,sizeof(struct Cyc_PP_Doc*),
4));});}goto _LL185;_LL1AA: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp26C=(struct
Cyc_Absyn_Sizeoftyp_e_struct*)_tmp23A;if(_tmp26C->tag != 18)goto _LL1AC;else{
_tmp26D=(void*)_tmp26C->f1;}}_LL1AB: s=({struct Cyc_PP_Doc*_tmp2D9[3];_tmp2D9[2]=
Cyc_PP_text(({const char*_tmp2DB=")";_tag_dyneither(_tmp2DB,sizeof(char),2);}));
_tmp2D9[1]=Cyc_Absynpp_typ2doc(_tmp26D);_tmp2D9[0]=Cyc_PP_text(({const char*
_tmp2DA="sizeof(";_tag_dyneither(_tmp2DA,sizeof(char),8);}));Cyc_PP_cat(
_tag_dyneither(_tmp2D9,sizeof(struct Cyc_PP_Doc*),3));});goto _LL185;_LL1AC: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp26E=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp23A;if(_tmp26E->tag != 19)goto _LL1AE;else{_tmp26F=_tmp26E->f1;}}_LL1AD: s=({
struct Cyc_PP_Doc*_tmp2DC[3];_tmp2DC[2]=Cyc_PP_text(({const char*_tmp2DE=")";
_tag_dyneither(_tmp2DE,sizeof(char),2);}));_tmp2DC[1]=Cyc_Absynpp_exp2doc(
_tmp26F);_tmp2DC[0]=Cyc_PP_text(({const char*_tmp2DD="sizeof(";_tag_dyneither(
_tmp2DD,sizeof(char),8);}));Cyc_PP_cat(_tag_dyneither(_tmp2DC,sizeof(struct Cyc_PP_Doc*),
3));});goto _LL185;_LL1AE: {struct Cyc_Absyn_Valueof_e_struct*_tmp270=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp23A;if(_tmp270->tag != 39)goto _LL1B0;else{_tmp271=(void*)_tmp270->f1;}}_LL1AF:
s=({struct Cyc_PP_Doc*_tmp2DF[3];_tmp2DF[2]=Cyc_PP_text(({const char*_tmp2E1=")";
_tag_dyneither(_tmp2E1,sizeof(char),2);}));_tmp2DF[1]=Cyc_Absynpp_typ2doc(
_tmp271);_tmp2DF[0]=Cyc_PP_text(({const char*_tmp2E0="valueof(";_tag_dyneither(
_tmp2E0,sizeof(char),9);}));Cyc_PP_cat(_tag_dyneither(_tmp2DF,sizeof(struct Cyc_PP_Doc*),
3));});goto _LL185;_LL1B0: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp272=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp23A;if(_tmp272->tag != 38)goto _LL1B2;else{_tmp273=_tmp272->f1;_tmp274=_tmp272->f2;}}
_LL1B1: s=({struct Cyc_PP_Doc*_tmp2E2[5];_tmp2E2[4]=Cyc_PP_text(({const char*
_tmp2E5=")";_tag_dyneither(_tmp2E5,sizeof(char),2);}));_tmp2E2[3]=Cyc_PP_textptr(
_tmp274);_tmp2E2[2]=Cyc_PP_text(({const char*_tmp2E4=".";_tag_dyneither(_tmp2E4,
sizeof(char),2);}));_tmp2E2[1]=Cyc_Absynpp_exp2doc(_tmp273);_tmp2E2[0]=Cyc_PP_text(({
const char*_tmp2E3="tagcheck(";_tag_dyneither(_tmp2E3,sizeof(char),10);}));Cyc_PP_cat(
_tag_dyneither(_tmp2E2,sizeof(struct Cyc_PP_Doc*),5));});goto _LL185;_LL1B2: {
struct Cyc_Absyn_Offsetof_e_struct*_tmp275=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp23A;if(_tmp275->tag != 20)goto _LL1B4;else{_tmp276=(void*)_tmp275->f1;_tmp277=(
void*)_tmp275->f2;{struct Cyc_Absyn_StructField_struct*_tmp278=(struct Cyc_Absyn_StructField_struct*)
_tmp277;if(_tmp278->tag != 0)goto _LL1B4;else{_tmp279=_tmp278->f1;}};}}_LL1B3: s=({
struct Cyc_PP_Doc*_tmp2E6[5];_tmp2E6[4]=Cyc_PP_text(({const char*_tmp2E9=")";
_tag_dyneither(_tmp2E9,sizeof(char),2);}));_tmp2E6[3]=Cyc_PP_textptr(_tmp279);
_tmp2E6[2]=Cyc_PP_text(({const char*_tmp2E8=",";_tag_dyneither(_tmp2E8,sizeof(
char),2);}));_tmp2E6[1]=Cyc_Absynpp_typ2doc(_tmp276);_tmp2E6[0]=Cyc_PP_text(({
const char*_tmp2E7="offsetof(";_tag_dyneither(_tmp2E7,sizeof(char),10);}));Cyc_PP_cat(
_tag_dyneither(_tmp2E6,sizeof(struct Cyc_PP_Doc*),5));});goto _LL185;_LL1B4: {
struct Cyc_Absyn_Offsetof_e_struct*_tmp27A=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp23A;if(_tmp27A->tag != 20)goto _LL1B6;else{_tmp27B=(void*)_tmp27A->f1;_tmp27C=(
void*)_tmp27A->f2;{struct Cyc_Absyn_TupleIndex_struct*_tmp27D=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp27C;if(_tmp27D->tag != 1)goto _LL1B6;else{_tmp27E=_tmp27D->f1;}};}}_LL1B5: s=({
struct Cyc_PP_Doc*_tmp2EA[5];_tmp2EA[4]=Cyc_PP_text(({const char*_tmp2F0=")";
_tag_dyneither(_tmp2F0,sizeof(char),2);}));_tmp2EA[3]=Cyc_PP_text((struct
_dyneither_ptr)({struct Cyc_Int_pa_struct _tmp2EF;_tmp2EF.tag=1;_tmp2EF.f1=(
unsigned long)((int)_tmp27E);({void*_tmp2ED[1]={& _tmp2EF};Cyc_aprintf(({const
char*_tmp2EE="%d";_tag_dyneither(_tmp2EE,sizeof(char),3);}),_tag_dyneither(
_tmp2ED,sizeof(void*),1));});}));_tmp2EA[2]=Cyc_PP_text(({const char*_tmp2EC=",";
_tag_dyneither(_tmp2EC,sizeof(char),2);}));_tmp2EA[1]=Cyc_Absynpp_typ2doc(
_tmp27B);_tmp2EA[0]=Cyc_PP_text(({const char*_tmp2EB="offsetof(";_tag_dyneither(
_tmp2EB,sizeof(char),10);}));Cyc_PP_cat(_tag_dyneither(_tmp2EA,sizeof(struct Cyc_PP_Doc*),
5));});goto _LL185;_LL1B6: {struct Cyc_Absyn_Deref_e_struct*_tmp27F=(struct Cyc_Absyn_Deref_e_struct*)
_tmp23A;if(_tmp27F->tag != 21)goto _LL1B8;else{_tmp280=_tmp27F->f1;}}_LL1B7: s=({
struct Cyc_PP_Doc*_tmp2F1[2];_tmp2F1[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp280);
_tmp2F1[0]=Cyc_PP_text(({const char*_tmp2F2="*";_tag_dyneither(_tmp2F2,sizeof(
char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp2F1,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL185;_LL1B8: {struct Cyc_Absyn_AggrMember_e_struct*_tmp281=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp23A;if(_tmp281->tag != 22)goto _LL1BA;else{_tmp282=_tmp281->f1;_tmp283=_tmp281->f2;}}
_LL1B9: s=({struct Cyc_PP_Doc*_tmp2F3[3];_tmp2F3[2]=Cyc_PP_textptr(_tmp283);
_tmp2F3[1]=Cyc_PP_text(({const char*_tmp2F4=".";_tag_dyneither(_tmp2F4,sizeof(
char),2);}));_tmp2F3[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp282);Cyc_PP_cat(
_tag_dyneither(_tmp2F3,sizeof(struct Cyc_PP_Doc*),3));});goto _LL185;_LL1BA: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp284=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp23A;if(_tmp284->tag != 23)goto _LL1BC;else{_tmp285=_tmp284->f1;_tmp286=_tmp284->f2;}}
_LL1BB: s=({struct Cyc_PP_Doc*_tmp2F5[3];_tmp2F5[2]=Cyc_PP_textptr(_tmp286);
_tmp2F5[1]=Cyc_PP_text(({const char*_tmp2F6="->";_tag_dyneither(_tmp2F6,sizeof(
char),3);}));_tmp2F5[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp285);Cyc_PP_cat(
_tag_dyneither(_tmp2F5,sizeof(struct Cyc_PP_Doc*),3));});goto _LL185;_LL1BC: {
struct Cyc_Absyn_Subscript_e_struct*_tmp287=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp23A;if(_tmp287->tag != 24)goto _LL1BE;else{_tmp288=_tmp287->f1;_tmp289=_tmp287->f2;}}
_LL1BD: s=({struct Cyc_PP_Doc*_tmp2F7[4];_tmp2F7[3]=Cyc_PP_text(({const char*
_tmp2F9="]";_tag_dyneither(_tmp2F9,sizeof(char),2);}));_tmp2F7[2]=Cyc_Absynpp_exp2doc(
_tmp289);_tmp2F7[1]=Cyc_PP_text(({const char*_tmp2F8="[";_tag_dyneither(_tmp2F8,
sizeof(char),2);}));_tmp2F7[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp288);Cyc_PP_cat(
_tag_dyneither(_tmp2F7,sizeof(struct Cyc_PP_Doc*),4));});goto _LL185;_LL1BE: {
struct Cyc_Absyn_Tuple_e_struct*_tmp28A=(struct Cyc_Absyn_Tuple_e_struct*)_tmp23A;
if(_tmp28A->tag != 25)goto _LL1C0;else{_tmp28B=_tmp28A->f1;}}_LL1BF: s=({struct Cyc_PP_Doc*
_tmp2FA[4];_tmp2FA[3]=Cyc_PP_text(({const char*_tmp2FC=")";_tag_dyneither(_tmp2FC,
sizeof(char),2);}));_tmp2FA[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp28B);_tmp2FA[1]=
Cyc_PP_text(({const char*_tmp2FB="(";_tag_dyneither(_tmp2FB,sizeof(char),2);}));
_tmp2FA[0]=Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_dyneither(_tmp2FA,sizeof(struct
Cyc_PP_Doc*),4));});goto _LL185;_LL1C0: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp28C=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp23A;if(_tmp28C->tag != 26)goto
_LL1C2;else{_tmp28D=_tmp28C->f1;_tmp28E=_tmp28C->f2;}}_LL1C1: s=({struct Cyc_PP_Doc*
_tmp2FD[4];_tmp2FD[3]=Cyc_Absynpp_group_braces(({const char*_tmp300=",";
_tag_dyneither(_tmp300,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp28E));_tmp2FD[2]=Cyc_PP_text(({const char*_tmp2FF=")";_tag_dyneither(_tmp2FF,
sizeof(char),2);}));_tmp2FD[1]=Cyc_Absynpp_typ2doc((*_tmp28D).f3);_tmp2FD[0]=Cyc_PP_text(({
const char*_tmp2FE="(";_tag_dyneither(_tmp2FE,sizeof(char),2);}));Cyc_PP_cat(
_tag_dyneither(_tmp2FD,sizeof(struct Cyc_PP_Doc*),4));});goto _LL185;_LL1C2: {
struct Cyc_Absyn_Array_e_struct*_tmp28F=(struct Cyc_Absyn_Array_e_struct*)_tmp23A;
if(_tmp28F->tag != 27)goto _LL1C4;else{_tmp290=_tmp28F->f1;}}_LL1C3: s=Cyc_Absynpp_group_braces(({
const char*_tmp301=",";_tag_dyneither(_tmp301,sizeof(char),2);}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp290));goto _LL185;_LL1C4: {struct Cyc_Absyn_Comprehension_e_struct*_tmp291=(
struct Cyc_Absyn_Comprehension_e_struct*)_tmp23A;if(_tmp291->tag != 28)goto _LL1C6;
else{_tmp292=_tmp291->f1;_tmp293=_tmp291->f2;_tmp294=_tmp291->f3;}}_LL1C5: s=({
struct Cyc_PP_Doc*_tmp302[8];_tmp302[7]=Cyc_Absynpp_rb();_tmp302[6]=Cyc_Absynpp_exp2doc(
_tmp294);_tmp302[5]=Cyc_PP_text(({const char*_tmp305=" : ";_tag_dyneither(_tmp305,
sizeof(char),4);}));_tmp302[4]=Cyc_Absynpp_exp2doc(_tmp293);_tmp302[3]=Cyc_PP_text(({
const char*_tmp304=" < ";_tag_dyneither(_tmp304,sizeof(char),4);}));_tmp302[2]=
Cyc_PP_text(*(*_tmp292->name).f2);_tmp302[1]=Cyc_PP_text(({const char*_tmp303="for ";
_tag_dyneither(_tmp303,sizeof(char),5);}));_tmp302[0]=Cyc_Absynpp_lb();Cyc_PP_cat(
_tag_dyneither(_tmp302,sizeof(struct Cyc_PP_Doc*),8));});goto _LL185;_LL1C6: {
struct Cyc_Absyn_Aggregate_e_struct*_tmp295=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp23A;if(_tmp295->tag != 29)goto _LL1C8;else{_tmp296=_tmp295->f1;_tmp297=_tmp295->f2;
_tmp298=_tmp295->f3;}}_LL1C7: {struct Cyc_List_List*_tmp306=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp298);s=({struct Cyc_PP_Doc*_tmp307[2];_tmp307[1]=Cyc_Absynpp_group_braces(({
const char*_tmp308=",";_tag_dyneither(_tmp308,sizeof(char),2);}),_tmp297 != 0?({
struct Cyc_List_List*_tmp309=_cycalloc(sizeof(*_tmp309));_tmp309->hd=Cyc_Absynpp_tps2doc(
_tmp297);_tmp309->tl=_tmp306;_tmp309;}): _tmp306);_tmp307[0]=Cyc_Absynpp_qvar2doc(
_tmp296);Cyc_PP_cat(_tag_dyneither(_tmp307,sizeof(struct Cyc_PP_Doc*),2));});goto
_LL185;}_LL1C8: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp299=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp23A;if(_tmp299->tag != 30)goto _LL1CA;else{_tmp29A=_tmp299->f2;}}_LL1C9: s=Cyc_Absynpp_group_braces(({
const char*_tmp30A=",";_tag_dyneither(_tmp30A,sizeof(char),2);}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp29A));goto _LL185;_LL1CA: {struct Cyc_Absyn_Datatype_e_struct*_tmp29B=(struct
Cyc_Absyn_Datatype_e_struct*)_tmp23A;if(_tmp29B->tag != 31)goto _LL1CC;else{
_tmp29C=_tmp29B->f1;_tmp29D=_tmp29B->f3;}}_LL1CB: if(_tmp29C == 0)s=Cyc_Absynpp_qvar2doc(
_tmp29D->name);else{s=({struct Cyc_PP_Doc*_tmp30B[2];_tmp30B[1]=Cyc_PP_egroup(({
const char*_tmp30C="(";_tag_dyneither(_tmp30C,sizeof(char),2);}),({const char*
_tmp30D=")";_tag_dyneither(_tmp30D,sizeof(char),2);}),({const char*_tmp30E=",";
_tag_dyneither(_tmp30E,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,
_tmp29C));_tmp30B[0]=Cyc_Absynpp_qvar2doc(_tmp29D->name);Cyc_PP_cat(
_tag_dyneither(_tmp30B,sizeof(struct Cyc_PP_Doc*),2));});}goto _LL185;_LL1CC: {
struct Cyc_Absyn_Enum_e_struct*_tmp29E=(struct Cyc_Absyn_Enum_e_struct*)_tmp23A;
if(_tmp29E->tag != 32)goto _LL1CE;else{_tmp29F=_tmp29E->f1;}}_LL1CD: s=Cyc_Absynpp_qvar2doc(
_tmp29F);goto _LL185;_LL1CE: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp2A0=(struct
Cyc_Absyn_AnonEnum_e_struct*)_tmp23A;if(_tmp2A0->tag != 33)goto _LL1D0;else{
_tmp2A1=_tmp2A0->f1;}}_LL1CF: s=Cyc_Absynpp_qvar2doc(_tmp2A1);goto _LL185;_LL1D0: {
struct Cyc_Absyn_Malloc_e_struct*_tmp2A2=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp23A;if(_tmp2A2->tag != 34)goto _LL1D2;else{_tmp2A3=_tmp2A2->f1;_tmp2A4=_tmp2A3.is_calloc;
_tmp2A5=_tmp2A3.rgn;_tmp2A6=_tmp2A3.elt_type;_tmp2A7=_tmp2A3.num_elts;}}_LL1D1:
if(_tmp2A4){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(
_tmp2A6)),0);if(_tmp2A5 == 0)s=({struct Cyc_PP_Doc*_tmp30F[5];_tmp30F[4]=Cyc_PP_text(({
const char*_tmp312=")";_tag_dyneither(_tmp312,sizeof(char),2);}));_tmp30F[3]=Cyc_Absynpp_exp2doc(
st);_tmp30F[2]=Cyc_PP_text(({const char*_tmp311=",";_tag_dyneither(_tmp311,
sizeof(char),2);}));_tmp30F[1]=Cyc_Absynpp_exp2doc(_tmp2A7);_tmp30F[0]=Cyc_PP_text(({
const char*_tmp310="calloc(";_tag_dyneither(_tmp310,sizeof(char),8);}));Cyc_PP_cat(
_tag_dyneither(_tmp30F,sizeof(struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*
_tmp313[7];_tmp313[6]=Cyc_PP_text(({const char*_tmp317=")";_tag_dyneither(_tmp317,
sizeof(char),2);}));_tmp313[5]=Cyc_Absynpp_exp2doc(st);_tmp313[4]=Cyc_PP_text(({
const char*_tmp316=",";_tag_dyneither(_tmp316,sizeof(char),2);}));_tmp313[3]=Cyc_Absynpp_exp2doc(
_tmp2A7);_tmp313[2]=Cyc_PP_text(({const char*_tmp315=",";_tag_dyneither(_tmp315,
sizeof(char),2);}));_tmp313[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp2A5);
_tmp313[0]=Cyc_PP_text(({const char*_tmp314="rcalloc(";_tag_dyneither(_tmp314,
sizeof(char),9);}));Cyc_PP_cat(_tag_dyneither(_tmp313,sizeof(struct Cyc_PP_Doc*),
7));});}}else{struct Cyc_Absyn_Exp*new_e;if(_tmp2A6 == 0)new_e=_tmp2A7;else{new_e=
Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmp2A6,0),_tmp2A7,0);}if(_tmp2A5 == 
0)s=({struct Cyc_PP_Doc*_tmp318[3];_tmp318[2]=Cyc_PP_text(({const char*_tmp31A=")";
_tag_dyneither(_tmp31A,sizeof(char),2);}));_tmp318[1]=Cyc_Absynpp_exp2doc(new_e);
_tmp318[0]=Cyc_PP_text(({const char*_tmp319="malloc(";_tag_dyneither(_tmp319,
sizeof(char),8);}));Cyc_PP_cat(_tag_dyneither(_tmp318,sizeof(struct Cyc_PP_Doc*),
3));});else{s=({struct Cyc_PP_Doc*_tmp31B[5];_tmp31B[4]=Cyc_PP_text(({const char*
_tmp31E=")";_tag_dyneither(_tmp31E,sizeof(char),2);}));_tmp31B[3]=Cyc_Absynpp_exp2doc(
new_e);_tmp31B[2]=Cyc_PP_text(({const char*_tmp31D=",";_tag_dyneither(_tmp31D,
sizeof(char),2);}));_tmp31B[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp2A5);
_tmp31B[0]=Cyc_PP_text(({const char*_tmp31C="rmalloc(";_tag_dyneither(_tmp31C,
sizeof(char),9);}));Cyc_PP_cat(_tag_dyneither(_tmp31B,sizeof(struct Cyc_PP_Doc*),
5));});}}goto _LL185;_LL1D2: {struct Cyc_Absyn_Swap_e_struct*_tmp2A8=(struct Cyc_Absyn_Swap_e_struct*)
_tmp23A;if(_tmp2A8->tag != 35)goto _LL1D4;else{_tmp2A9=_tmp2A8->f1;_tmp2AA=_tmp2A8->f2;}}
_LL1D3: s=({struct Cyc_PP_Doc*_tmp31F[5];_tmp31F[4]=Cyc_PP_text(({const char*
_tmp322=")";_tag_dyneither(_tmp322,sizeof(char),2);}));_tmp31F[3]=Cyc_Absynpp_exp2doc(
_tmp2AA);_tmp31F[2]=Cyc_PP_text(({const char*_tmp321=",";_tag_dyneither(_tmp321,
sizeof(char),2);}));_tmp31F[1]=Cyc_Absynpp_exp2doc(_tmp2A9);_tmp31F[0]=Cyc_PP_text(({
const char*_tmp320="cycswap(";_tag_dyneither(_tmp320,sizeof(char),9);}));Cyc_PP_cat(
_tag_dyneither(_tmp31F,sizeof(struct Cyc_PP_Doc*),5));});goto _LL185;_LL1D4: {
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp2AB=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp23A;if(_tmp2AB->tag != 36)goto _LL1D6;else{_tmp2AC=_tmp2AB->f1;_tmp2AD=_tmp2AB->f2;}}
_LL1D5: s=Cyc_Absynpp_group_braces(({const char*_tmp323=",";_tag_dyneither(_tmp323,
sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2AD));
goto _LL185;_LL1D6: {struct Cyc_Absyn_StmtExp_e_struct*_tmp2AE=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp23A;if(_tmp2AE->tag != 37)goto _LL185;else{_tmp2AF=_tmp2AE->f1;}}_LL1D7: s=({
struct Cyc_PP_Doc*_tmp324[7];_tmp324[6]=Cyc_PP_text(({const char*_tmp326=")";
_tag_dyneither(_tmp326,sizeof(char),2);}));_tmp324[5]=Cyc_Absynpp_rb();_tmp324[4]=
Cyc_PP_blank_doc();_tmp324[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2AF,1));
_tmp324[2]=Cyc_PP_blank_doc();_tmp324[1]=Cyc_Absynpp_lb();_tmp324[0]=Cyc_PP_text(({
const char*_tmp325="(";_tag_dyneither(_tmp325,sizeof(char),2);}));Cyc_PP_cat(
_tag_dyneither(_tmp324,sizeof(struct Cyc_PP_Doc*),7));});goto _LL185;_LL185:;}if(
inprec >= myprec)s=({struct Cyc_PP_Doc*_tmp327[3];_tmp327[2]=Cyc_PP_text(({const
char*_tmp329=")";_tag_dyneither(_tmp329,sizeof(char),2);}));_tmp327[1]=s;_tmp327[
0]=Cyc_PP_text(({const char*_tmp328="(";_tag_dyneither(_tmp328,sizeof(char),2);}));
Cyc_PP_cat(_tag_dyneither(_tmp327,sizeof(struct Cyc_PP_Doc*),3));});return s;}
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){void*_tmp32A=d;struct Cyc_Absyn_Exp*
_tmp32C;struct _dyneither_ptr*_tmp32E;_LL1DE: {struct Cyc_Absyn_ArrayElement_struct*
_tmp32B=(struct Cyc_Absyn_ArrayElement_struct*)_tmp32A;if(_tmp32B->tag != 0)goto
_LL1E0;else{_tmp32C=_tmp32B->f1;}}_LL1DF: return({struct Cyc_PP_Doc*_tmp32F[3];
_tmp32F[2]=Cyc_PP_text(({const char*_tmp331="]";_tag_dyneither(_tmp331,sizeof(
char),2);}));_tmp32F[1]=Cyc_Absynpp_exp2doc(_tmp32C);_tmp32F[0]=Cyc_PP_text(({
const char*_tmp330=".[";_tag_dyneither(_tmp330,sizeof(char),3);}));Cyc_PP_cat(
_tag_dyneither(_tmp32F,sizeof(struct Cyc_PP_Doc*),3));});_LL1E0: {struct Cyc_Absyn_FieldName_struct*
_tmp32D=(struct Cyc_Absyn_FieldName_struct*)_tmp32A;if(_tmp32D->tag != 1)goto
_LL1DD;else{_tmp32E=_tmp32D->f1;}}_LL1E1: return({struct Cyc_PP_Doc*_tmp332[2];
_tmp332[1]=Cyc_PP_textptr(_tmp32E);_tmp332[0]=Cyc_PP_text(({const char*_tmp333=".";
_tag_dyneither(_tmp333,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp332,
sizeof(struct Cyc_PP_Doc*),2));});_LL1DD:;}struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(
struct _tuple14*de){if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
return({struct Cyc_PP_Doc*_tmp334[2];_tmp334[1]=Cyc_Absynpp_exp2doc((*de).f2);
_tmp334[0]=Cyc_PP_egroup(({const char*_tmp335="";_tag_dyneither(_tmp335,sizeof(
char),1);}),({const char*_tmp336="=";_tag_dyneither(_tmp336,sizeof(char),2);}),({
const char*_tmp337="=";_tag_dyneither(_tmp337,sizeof(char),2);}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*
de).f1));Cyc_PP_cat(_tag_dyneither(_tmp334,sizeof(struct Cyc_PP_Doc*),2));});}}
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
return Cyc_PP_group(({const char*_tmp338="";_tag_dyneither(_tmp338,sizeof(char),1);}),({
const char*_tmp339="";_tag_dyneither(_tmp339,sizeof(char),1);}),({const char*
_tmp33A=",";_tag_dyneither(_tmp33A,sizeof(char),2);}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))
Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(
union Cyc_Absyn_Cnst c){union Cyc_Absyn_Cnst _tmp33B=c;struct _tuple3 _tmp33C;enum Cyc_Absyn_Sign
_tmp33D;char _tmp33E;struct _tuple4 _tmp33F;enum Cyc_Absyn_Sign _tmp340;short _tmp341;
struct _tuple5 _tmp342;enum Cyc_Absyn_Sign _tmp343;int _tmp344;struct _tuple5 _tmp345;
enum Cyc_Absyn_Sign _tmp346;int _tmp347;struct _tuple5 _tmp348;enum Cyc_Absyn_Sign
_tmp349;int _tmp34A;struct _tuple6 _tmp34B;enum Cyc_Absyn_Sign _tmp34C;long long
_tmp34D;struct _dyneither_ptr _tmp34E;int _tmp34F;struct _dyneither_ptr _tmp350;
_LL1E3: if((_tmp33B.Char_c).tag != 2)goto _LL1E5;_tmp33C=(struct _tuple3)(_tmp33B.Char_c).val;
_tmp33D=_tmp33C.f1;_tmp33E=_tmp33C.f2;_LL1E4: return Cyc_PP_text((struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp353;_tmp353.tag=0;_tmp353.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp33E));({
void*_tmp351[1]={& _tmp353};Cyc_aprintf(({const char*_tmp352="'%s'";_tag_dyneither(
_tmp352,sizeof(char),5);}),_tag_dyneither(_tmp351,sizeof(void*),1));});}));
_LL1E5: if((_tmp33B.Short_c).tag != 3)goto _LL1E7;_tmp33F=(struct _tuple4)(_tmp33B.Short_c).val;
_tmp340=_tmp33F.f1;_tmp341=_tmp33F.f2;_LL1E6: return Cyc_PP_text((struct
_dyneither_ptr)({struct Cyc_Int_pa_struct _tmp356;_tmp356.tag=1;_tmp356.f1=(
unsigned long)((int)_tmp341);({void*_tmp354[1]={& _tmp356};Cyc_aprintf(({const
char*_tmp355="%d";_tag_dyneither(_tmp355,sizeof(char),3);}),_tag_dyneither(
_tmp354,sizeof(void*),1));});}));_LL1E7: if((_tmp33B.Int_c).tag != 4)goto _LL1E9;
_tmp342=(struct _tuple5)(_tmp33B.Int_c).val;_tmp343=_tmp342.f1;if(_tmp343 != Cyc_Absyn_None)
goto _LL1E9;_tmp344=_tmp342.f2;_LL1E8: _tmp347=_tmp344;goto _LL1EA;_LL1E9: if((
_tmp33B.Int_c).tag != 4)goto _LL1EB;_tmp345=(struct _tuple5)(_tmp33B.Int_c).val;
_tmp346=_tmp345.f1;if(_tmp346 != Cyc_Absyn_Signed)goto _LL1EB;_tmp347=_tmp345.f2;
_LL1EA: return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp359;
_tmp359.tag=1;_tmp359.f1=(unsigned long)_tmp347;({void*_tmp357[1]={& _tmp359};Cyc_aprintf(({
const char*_tmp358="%d";_tag_dyneither(_tmp358,sizeof(char),3);}),_tag_dyneither(
_tmp357,sizeof(void*),1));});}));_LL1EB: if((_tmp33B.Int_c).tag != 4)goto _LL1ED;
_tmp348=(struct _tuple5)(_tmp33B.Int_c).val;_tmp349=_tmp348.f1;if(_tmp349 != Cyc_Absyn_Unsigned)
goto _LL1ED;_tmp34A=_tmp348.f2;_LL1EC: return Cyc_PP_text((struct _dyneither_ptr)({
struct Cyc_Int_pa_struct _tmp35C;_tmp35C.tag=1;_tmp35C.f1=(unsigned int)_tmp34A;({
void*_tmp35A[1]={& _tmp35C};Cyc_aprintf(({const char*_tmp35B="%u";_tag_dyneither(
_tmp35B,sizeof(char),3);}),_tag_dyneither(_tmp35A,sizeof(void*),1));});}));
_LL1ED: if((_tmp33B.LongLong_c).tag != 5)goto _LL1EF;_tmp34B=(struct _tuple6)(
_tmp33B.LongLong_c).val;_tmp34C=_tmp34B.f1;_tmp34D=_tmp34B.f2;_LL1EE: return Cyc_PP_text(({
const char*_tmp35D="<<FIX LONG LONG CONSTANT>>";_tag_dyneither(_tmp35D,sizeof(
char),27);}));_LL1EF: if((_tmp33B.Float_c).tag != 6)goto _LL1F1;_tmp34E=(struct
_dyneither_ptr)(_tmp33B.Float_c).val;_LL1F0: return Cyc_PP_text(_tmp34E);_LL1F1:
if((_tmp33B.Null_c).tag != 1)goto _LL1F3;_tmp34F=(int)(_tmp33B.Null_c).val;_LL1F2:
return Cyc_PP_text(({const char*_tmp35E="NULL";_tag_dyneither(_tmp35E,sizeof(char),
5);}));_LL1F3: if((_tmp33B.String_c).tag != 7)goto _LL1E2;_tmp350=(struct
_dyneither_ptr)(_tmp33B.String_c).val;_LL1F4: return({struct Cyc_PP_Doc*_tmp35F[3];
_tmp35F[2]=Cyc_PP_text(({const char*_tmp361="\"";_tag_dyneither(_tmp361,sizeof(
char),2);}));_tmp35F[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp350));_tmp35F[
0]=Cyc_PP_text(({const char*_tmp360="\"";_tag_dyneither(_tmp360,sizeof(char),2);}));
Cyc_PP_cat(_tag_dyneither(_tmp35F,sizeof(struct Cyc_PP_Doc*),3));});_LL1E2:;}
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct
Cyc_List_List*es){struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);if(p == (enum Cyc_Absyn_Primop)
Cyc_Absyn_Numelts){if(es == 0  || es->tl != 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp362=_cycalloc(sizeof(*_tmp362));_tmp362[0]=({struct Cyc_Core_Failure_struct
_tmp363;_tmp363.tag=Cyc_Core_Failure;_tmp363.f1=(struct _dyneither_ptr)({struct
Cyc_String_pa_struct _tmp366;_tmp366.tag=0;_tmp366.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72));({void*_tmp364[1]={& _tmp366};
Cyc_aprintf(({const char*_tmp365="Absynpp::primapp2doc Numelts: %s with bad args";
_tag_dyneither(_tmp365,sizeof(char),47);}),_tag_dyneither(_tmp364,sizeof(void*),
1));});});_tmp363;});_tmp362;}));return({struct Cyc_PP_Doc*_tmp367[3];_tmp367[2]=
Cyc_PP_text(({const char*_tmp369=")";_tag_dyneither(_tmp369,sizeof(char),2);}));
_tmp367[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd);_tmp367[0]=Cyc_PP_text(({
const char*_tmp368="numelts(";_tag_dyneither(_tmp368,sizeof(char),9);}));Cyc_PP_cat(
_tag_dyneither(_tmp367,sizeof(struct Cyc_PP_Doc*),3));});}else{struct Cyc_List_List*
ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp36A=_cycalloc(
sizeof(*_tmp36A));_tmp36A[0]=({struct Cyc_Core_Failure_struct _tmp36B;_tmp36B.tag=
Cyc_Core_Failure;_tmp36B.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp36E;_tmp36E.tag=0;_tmp36E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_PP_string_of_doc(ps,72));({void*_tmp36C[1]={& _tmp36E};Cyc_aprintf(({const
char*_tmp36D="Absynpp::primapp2doc: %s with no args";_tag_dyneither(_tmp36D,
sizeof(char),38);}),_tag_dyneither(_tmp36C,sizeof(void*),1));});});_tmp36B;});
_tmp36A;}));else{if(ds->tl == 0)return({struct Cyc_PP_Doc*_tmp36F[3];_tmp36F[2]=(
struct Cyc_PP_Doc*)ds->hd;_tmp36F[1]=Cyc_PP_text(({const char*_tmp370=" ";
_tag_dyneither(_tmp370,sizeof(char),2);}));_tmp36F[0]=ps;Cyc_PP_cat(
_tag_dyneither(_tmp36F,sizeof(struct Cyc_PP_Doc*),3));});else{if(((struct Cyc_List_List*)
_check_null(ds->tl))->tl != 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp371=_cycalloc(sizeof(*_tmp371));_tmp371[0]=({struct Cyc_Core_Failure_struct
_tmp372;_tmp372.tag=Cyc_Core_Failure;_tmp372.f1=(struct _dyneither_ptr)({struct
Cyc_String_pa_struct _tmp375;_tmp375.tag=0;_tmp375.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72));({void*_tmp373[1]={& _tmp375};
Cyc_aprintf(({const char*_tmp374="Absynpp::primapp2doc: %s with more than 2 args";
_tag_dyneither(_tmp374,sizeof(char),47);}),_tag_dyneither(_tmp373,sizeof(void*),
1));});});_tmp372;});_tmp371;}));else{return({struct Cyc_PP_Doc*_tmp376[5];
_tmp376[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd;
_tmp376[3]=Cyc_PP_text(({const char*_tmp378=" ";_tag_dyneither(_tmp378,sizeof(
char),2);}));_tmp376[2]=ps;_tmp376[1]=Cyc_PP_text(({const char*_tmp377=" ";
_tag_dyneither(_tmp377,sizeof(char),2);}));_tmp376[0]=(struct Cyc_PP_Doc*)ds->hd;
Cyc_PP_cat(_tag_dyneither(_tmp376,sizeof(struct Cyc_PP_Doc*),5));});}}}}}struct
_dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){switch(p){case Cyc_Absyn_Plus:
_LL1F5: return({const char*_tmp379="+";_tag_dyneither(_tmp379,sizeof(char),2);});
case Cyc_Absyn_Times: _LL1F6: return({const char*_tmp37A="*";_tag_dyneither(_tmp37A,
sizeof(char),2);});case Cyc_Absyn_Minus: _LL1F7: return({const char*_tmp37B="-";
_tag_dyneither(_tmp37B,sizeof(char),2);});case Cyc_Absyn_Div: _LL1F8: return({const
char*_tmp37C="/";_tag_dyneither(_tmp37C,sizeof(char),2);});case Cyc_Absyn_Mod:
_LL1F9: return Cyc_Absynpp_print_for_cycdoc?({const char*_tmp37D="\\%";
_tag_dyneither(_tmp37D,sizeof(char),3);}):({const char*_tmp37E="%";_tag_dyneither(
_tmp37E,sizeof(char),2);});case Cyc_Absyn_Eq: _LL1FA: return({const char*_tmp37F="==";
_tag_dyneither(_tmp37F,sizeof(char),3);});case Cyc_Absyn_Neq: _LL1FB: return({const
char*_tmp380="!=";_tag_dyneither(_tmp380,sizeof(char),3);});case Cyc_Absyn_Gt:
_LL1FC: return({const char*_tmp381=">";_tag_dyneither(_tmp381,sizeof(char),2);});
case Cyc_Absyn_Lt: _LL1FD: return({const char*_tmp382="<";_tag_dyneither(_tmp382,
sizeof(char),2);});case Cyc_Absyn_Gte: _LL1FE: return({const char*_tmp383=">=";
_tag_dyneither(_tmp383,sizeof(char),3);});case Cyc_Absyn_Lte: _LL1FF: return({const
char*_tmp384="<=";_tag_dyneither(_tmp384,sizeof(char),3);});case Cyc_Absyn_Not:
_LL200: return({const char*_tmp385="!";_tag_dyneither(_tmp385,sizeof(char),2);});
case Cyc_Absyn_Bitnot: _LL201: return({const char*_tmp386="~";_tag_dyneither(_tmp386,
sizeof(char),2);});case Cyc_Absyn_Bitand: _LL202: return({const char*_tmp387="&";
_tag_dyneither(_tmp387,sizeof(char),2);});case Cyc_Absyn_Bitor: _LL203: return({
const char*_tmp388="|";_tag_dyneither(_tmp388,sizeof(char),2);});case Cyc_Absyn_Bitxor:
_LL204: return({const char*_tmp389="^";_tag_dyneither(_tmp389,sizeof(char),2);});
case Cyc_Absyn_Bitlshift: _LL205: return({const char*_tmp38A="<<";_tag_dyneither(
_tmp38A,sizeof(char),3);});case Cyc_Absyn_Bitlrshift: _LL206: return({const char*
_tmp38B=">>";_tag_dyneither(_tmp38B,sizeof(char),3);});case Cyc_Absyn_Bitarshift:
_LL207: return({const char*_tmp38C=">>>";_tag_dyneither(_tmp38C,sizeof(char),4);});
case Cyc_Absyn_Numelts: _LL208: return({const char*_tmp38D="numelts";_tag_dyneither(
_tmp38D,sizeof(char),8);});}}struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop
p){return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s){void*_tmp38E=s->r;_LL20B: {struct Cyc_Absyn_Decl_s_struct*
_tmp38F=(struct Cyc_Absyn_Decl_s_struct*)_tmp38E;if(_tmp38F->tag != 12)goto _LL20D;}
_LL20C: return 1;_LL20D:;_LL20E: return 0;_LL20A:;}struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(
struct Cyc_Absyn_Stmt*st,int stmtexp){struct Cyc_PP_Doc*s;{void*_tmp390=st->r;
struct Cyc_Absyn_Exp*_tmp393;struct Cyc_Absyn_Stmt*_tmp395;struct Cyc_Absyn_Stmt*
_tmp396;struct Cyc_Absyn_Exp*_tmp398;struct Cyc_Absyn_Exp*_tmp39A;struct Cyc_Absyn_Stmt*
_tmp39B;struct Cyc_Absyn_Stmt*_tmp39C;struct _tuple8 _tmp39E;struct Cyc_Absyn_Exp*
_tmp39F;struct Cyc_Absyn_Stmt*_tmp3A0;struct _dyneither_ptr*_tmp3A4;struct Cyc_Absyn_Exp*
_tmp3A6;struct _tuple8 _tmp3A7;struct Cyc_Absyn_Exp*_tmp3A8;struct _tuple8 _tmp3A9;
struct Cyc_Absyn_Exp*_tmp3AA;struct Cyc_Absyn_Stmt*_tmp3AB;struct Cyc_Absyn_Exp*
_tmp3AD;struct Cyc_List_List*_tmp3AE;struct Cyc_List_List*_tmp3B0;struct Cyc_List_List*
_tmp3B2;struct Cyc_Absyn_Decl*_tmp3B4;struct Cyc_Absyn_Stmt*_tmp3B5;struct
_dyneither_ptr*_tmp3B7;struct Cyc_Absyn_Stmt*_tmp3B8;struct Cyc_Absyn_Stmt*_tmp3BA;
struct _tuple8 _tmp3BB;struct Cyc_Absyn_Exp*_tmp3BC;struct Cyc_Absyn_Stmt*_tmp3BE;
struct Cyc_List_List*_tmp3BF;struct Cyc_Absyn_Exp*_tmp3C1;_LL210: {struct Cyc_Absyn_Skip_s_struct*
_tmp391=(struct Cyc_Absyn_Skip_s_struct*)_tmp390;if(_tmp391->tag != 0)goto _LL212;}
_LL211: s=Cyc_PP_text(({const char*_tmp3C2=";";_tag_dyneither(_tmp3C2,sizeof(char),
2);}));goto _LL20F;_LL212: {struct Cyc_Absyn_Exp_s_struct*_tmp392=(struct Cyc_Absyn_Exp_s_struct*)
_tmp390;if(_tmp392->tag != 1)goto _LL214;else{_tmp393=_tmp392->f1;}}_LL213: s=({
struct Cyc_PP_Doc*_tmp3C3[2];_tmp3C3[1]=Cyc_PP_text(({const char*_tmp3C4=";";
_tag_dyneither(_tmp3C4,sizeof(char),2);}));_tmp3C3[0]=Cyc_Absynpp_exp2doc(
_tmp393);Cyc_PP_cat(_tag_dyneither(_tmp3C3,sizeof(struct Cyc_PP_Doc*),2));});goto
_LL20F;_LL214: {struct Cyc_Absyn_Seq_s_struct*_tmp394=(struct Cyc_Absyn_Seq_s_struct*)
_tmp390;if(_tmp394->tag != 2)goto _LL216;else{_tmp395=_tmp394->f1;_tmp396=_tmp394->f2;}}
_LL215: if(Cyc_Absynpp_decls_first){if(Cyc_Absynpp_is_declaration(_tmp395))s=({
struct Cyc_PP_Doc*_tmp3C5[7];_tmp3C5[6]=Cyc_Absynpp_is_declaration(_tmp396)?
stmtexp?({struct Cyc_PP_Doc*_tmp3C6[7];_tmp3C6[6]=Cyc_PP_line_doc();_tmp3C6[5]=
Cyc_PP_text(({const char*_tmp3C8=");";_tag_dyneither(_tmp3C8,sizeof(char),3);}));
_tmp3C6[4]=Cyc_Absynpp_rb();_tmp3C6[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp396,stmtexp));_tmp3C6[2]=Cyc_PP_blank_doc();_tmp3C6[1]=Cyc_Absynpp_lb();
_tmp3C6[0]=Cyc_PP_text(({const char*_tmp3C7="(";_tag_dyneither(_tmp3C7,sizeof(
char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp3C6,sizeof(struct Cyc_PP_Doc*),7));}):({
struct Cyc_PP_Doc*_tmp3C9[5];_tmp3C9[4]=Cyc_PP_line_doc();_tmp3C9[3]=Cyc_Absynpp_rb();
_tmp3C9[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp396,stmtexp));_tmp3C9[1]=Cyc_PP_blank_doc();
_tmp3C9[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_dyneither(_tmp3C9,sizeof(struct Cyc_PP_Doc*),
5));}): Cyc_Absynpp_stmt2doc(_tmp396,stmtexp);_tmp3C5[5]=Cyc_PP_line_doc();
_tmp3C5[4]=Cyc_Absynpp_rb();_tmp3C5[3]=Cyc_PP_line_doc();_tmp3C5[2]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp395,0));_tmp3C5[1]=Cyc_PP_blank_doc();_tmp3C5[0]=Cyc_Absynpp_lb();
Cyc_PP_cat(_tag_dyneither(_tmp3C5,sizeof(struct Cyc_PP_Doc*),7));});else{if(Cyc_Absynpp_is_declaration(
_tmp396))s=({struct Cyc_PP_Doc*_tmp3CA[4];_tmp3CA[3]=Cyc_PP_line_doc();_tmp3CA[2]=
stmtexp?({struct Cyc_PP_Doc*_tmp3CB[6];_tmp3CB[5]=Cyc_PP_text(({const char*_tmp3CD=");";
_tag_dyneither(_tmp3CD,sizeof(char),3);}));_tmp3CB[4]=Cyc_Absynpp_rb();_tmp3CB[3]=
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp396,stmtexp));_tmp3CB[2]=Cyc_PP_blank_doc();
_tmp3CB[1]=Cyc_Absynpp_lb();_tmp3CB[0]=Cyc_PP_text(({const char*_tmp3CC="(";
_tag_dyneither(_tmp3CC,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp3CB,
sizeof(struct Cyc_PP_Doc*),6));}):({struct Cyc_PP_Doc*_tmp3CE[4];_tmp3CE[3]=Cyc_Absynpp_rb();
_tmp3CE[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp396,stmtexp));_tmp3CE[1]=Cyc_PP_blank_doc();
_tmp3CE[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_dyneither(_tmp3CE,sizeof(struct Cyc_PP_Doc*),
4));});_tmp3CA[1]=Cyc_PP_line_doc();_tmp3CA[0]=Cyc_Absynpp_stmt2doc(_tmp395,0);
Cyc_PP_cat(_tag_dyneither(_tmp3CA,sizeof(struct Cyc_PP_Doc*),4));});else{s=({
struct Cyc_PP_Doc*_tmp3CF[3];_tmp3CF[2]=Cyc_Absynpp_stmt2doc(_tmp396,stmtexp);
_tmp3CF[1]=Cyc_PP_line_doc();_tmp3CF[0]=Cyc_Absynpp_stmt2doc(_tmp395,0);Cyc_PP_cat(
_tag_dyneither(_tmp3CF,sizeof(struct Cyc_PP_Doc*),3));});}}}else{s=({struct Cyc_PP_Doc*
_tmp3D0[3];_tmp3D0[2]=Cyc_Absynpp_stmt2doc(_tmp396,stmtexp);_tmp3D0[1]=Cyc_PP_line_doc();
_tmp3D0[0]=Cyc_Absynpp_stmt2doc(_tmp395,0);Cyc_PP_cat(_tag_dyneither(_tmp3D0,
sizeof(struct Cyc_PP_Doc*),3));});}goto _LL20F;_LL216: {struct Cyc_Absyn_Return_s_struct*
_tmp397=(struct Cyc_Absyn_Return_s_struct*)_tmp390;if(_tmp397->tag != 3)goto _LL218;
else{_tmp398=_tmp397->f1;}}_LL217: if(_tmp398 == 0)s=Cyc_PP_text(({const char*
_tmp3D1="return;";_tag_dyneither(_tmp3D1,sizeof(char),8);}));else{s=({struct Cyc_PP_Doc*
_tmp3D2[3];_tmp3D2[2]=Cyc_PP_text(({const char*_tmp3D4=";";_tag_dyneither(_tmp3D4,
sizeof(char),2);}));_tmp3D2[1]=_tmp398 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp398);_tmp3D2[0]=Cyc_PP_text(({const char*_tmp3D3="return ";
_tag_dyneither(_tmp3D3,sizeof(char),8);}));Cyc_PP_cat(_tag_dyneither(_tmp3D2,
sizeof(struct Cyc_PP_Doc*),3));});}goto _LL20F;_LL218: {struct Cyc_Absyn_IfThenElse_s_struct*
_tmp399=(struct Cyc_Absyn_IfThenElse_s_struct*)_tmp390;if(_tmp399->tag != 4)goto
_LL21A;else{_tmp39A=_tmp399->f1;_tmp39B=_tmp399->f2;_tmp39C=_tmp399->f3;}}_LL219: {
int print_else;{void*_tmp3D5=_tmp39C->r;_LL235: {struct Cyc_Absyn_Skip_s_struct*
_tmp3D6=(struct Cyc_Absyn_Skip_s_struct*)_tmp3D5;if(_tmp3D6->tag != 0)goto _LL237;}
_LL236: print_else=0;goto _LL234;_LL237:;_LL238: print_else=1;goto _LL234;_LL234:;}s=({
struct Cyc_PP_Doc*_tmp3D7[8];_tmp3D7[7]=print_else?({struct Cyc_PP_Doc*_tmp3DB[6];
_tmp3DB[5]=Cyc_Absynpp_rb();_tmp3DB[4]=Cyc_PP_line_doc();_tmp3DB[3]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp3DD[2];_tmp3DD[1]=Cyc_Absynpp_stmt2doc(_tmp39C,0);
_tmp3DD[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp3DD,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp3DB[2]=Cyc_Absynpp_lb();_tmp3DB[1]=Cyc_PP_text(({const char*_tmp3DC="else ";
_tag_dyneither(_tmp3DC,sizeof(char),6);}));_tmp3DB[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_dyneither(_tmp3DB,sizeof(struct Cyc_PP_Doc*),6));}): Cyc_PP_nil_doc();_tmp3D7[
6]=Cyc_Absynpp_rb();_tmp3D7[5]=Cyc_PP_line_doc();_tmp3D7[4]=Cyc_PP_nest(2,({
struct Cyc_PP_Doc*_tmp3DA[2];_tmp3DA[1]=Cyc_Absynpp_stmt2doc(_tmp39B,0);_tmp3DA[0]=
Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp3DA,sizeof(struct Cyc_PP_Doc*),2));}));
_tmp3D7[3]=Cyc_Absynpp_lb();_tmp3D7[2]=Cyc_PP_text(({const char*_tmp3D9=") ";
_tag_dyneither(_tmp3D9,sizeof(char),3);}));_tmp3D7[1]=Cyc_Absynpp_exp2doc(
_tmp39A);_tmp3D7[0]=Cyc_PP_text(({const char*_tmp3D8="if (";_tag_dyneither(
_tmp3D8,sizeof(char),5);}));Cyc_PP_cat(_tag_dyneither(_tmp3D7,sizeof(struct Cyc_PP_Doc*),
8));});goto _LL20F;}_LL21A: {struct Cyc_Absyn_While_s_struct*_tmp39D=(struct Cyc_Absyn_While_s_struct*)
_tmp390;if(_tmp39D->tag != 5)goto _LL21C;else{_tmp39E=_tmp39D->f1;_tmp39F=_tmp39E.f1;
_tmp3A0=_tmp39D->f2;}}_LL21B: s=({struct Cyc_PP_Doc*_tmp3DE[7];_tmp3DE[6]=Cyc_Absynpp_rb();
_tmp3DE[5]=Cyc_PP_line_doc();_tmp3DE[4]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3E1[
2];_tmp3E1[1]=Cyc_Absynpp_stmt2doc(_tmp3A0,0);_tmp3E1[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_dyneither(_tmp3E1,sizeof(struct Cyc_PP_Doc*),2));}));_tmp3DE[3]=Cyc_Absynpp_lb();
_tmp3DE[2]=Cyc_PP_text(({const char*_tmp3E0=") ";_tag_dyneither(_tmp3E0,sizeof(
char),3);}));_tmp3DE[1]=Cyc_Absynpp_exp2doc(_tmp39F);_tmp3DE[0]=Cyc_PP_text(({
const char*_tmp3DF="while (";_tag_dyneither(_tmp3DF,sizeof(char),8);}));Cyc_PP_cat(
_tag_dyneither(_tmp3DE,sizeof(struct Cyc_PP_Doc*),7));});goto _LL20F;_LL21C: {
struct Cyc_Absyn_Break_s_struct*_tmp3A1=(struct Cyc_Absyn_Break_s_struct*)_tmp390;
if(_tmp3A1->tag != 6)goto _LL21E;}_LL21D: s=Cyc_PP_text(({const char*_tmp3E2="break;";
_tag_dyneither(_tmp3E2,sizeof(char),7);}));goto _LL20F;_LL21E: {struct Cyc_Absyn_Continue_s_struct*
_tmp3A2=(struct Cyc_Absyn_Continue_s_struct*)_tmp390;if(_tmp3A2->tag != 7)goto
_LL220;}_LL21F: s=Cyc_PP_text(({const char*_tmp3E3="continue;";_tag_dyneither(
_tmp3E3,sizeof(char),10);}));goto _LL20F;_LL220: {struct Cyc_Absyn_Goto_s_struct*
_tmp3A3=(struct Cyc_Absyn_Goto_s_struct*)_tmp390;if(_tmp3A3->tag != 8)goto _LL222;
else{_tmp3A4=_tmp3A3->f1;}}_LL221: s=Cyc_PP_text((struct _dyneither_ptr)({struct
Cyc_String_pa_struct _tmp3E6;_tmp3E6.tag=0;_tmp3E6.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp3A4);({void*_tmp3E4[1]={& _tmp3E6};Cyc_aprintf(({const
char*_tmp3E5="goto %s;";_tag_dyneither(_tmp3E5,sizeof(char),9);}),_tag_dyneither(
_tmp3E4,sizeof(void*),1));});}));goto _LL20F;_LL222: {struct Cyc_Absyn_For_s_struct*
_tmp3A5=(struct Cyc_Absyn_For_s_struct*)_tmp390;if(_tmp3A5->tag != 9)goto _LL224;
else{_tmp3A6=_tmp3A5->f1;_tmp3A7=_tmp3A5->f2;_tmp3A8=_tmp3A7.f1;_tmp3A9=_tmp3A5->f3;
_tmp3AA=_tmp3A9.f1;_tmp3AB=_tmp3A5->f4;}}_LL223: s=({struct Cyc_PP_Doc*_tmp3E7[11];
_tmp3E7[10]=Cyc_Absynpp_rb();_tmp3E7[9]=Cyc_PP_line_doc();_tmp3E7[8]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp3EC[2];_tmp3EC[1]=Cyc_Absynpp_stmt2doc(_tmp3AB,0);
_tmp3EC[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp3EC,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp3E7[7]=Cyc_Absynpp_lb();_tmp3E7[6]=Cyc_PP_text(({const char*_tmp3EB=") ";
_tag_dyneither(_tmp3EB,sizeof(char),3);}));_tmp3E7[5]=Cyc_Absynpp_exp2doc(
_tmp3AA);_tmp3E7[4]=Cyc_PP_text(({const char*_tmp3EA="; ";_tag_dyneither(_tmp3EA,
sizeof(char),3);}));_tmp3E7[3]=Cyc_Absynpp_exp2doc(_tmp3A8);_tmp3E7[2]=Cyc_PP_text(({
const char*_tmp3E9="; ";_tag_dyneither(_tmp3E9,sizeof(char),3);}));_tmp3E7[1]=Cyc_Absynpp_exp2doc(
_tmp3A6);_tmp3E7[0]=Cyc_PP_text(({const char*_tmp3E8="for(";_tag_dyneither(
_tmp3E8,sizeof(char),5);}));Cyc_PP_cat(_tag_dyneither(_tmp3E7,sizeof(struct Cyc_PP_Doc*),
11));});goto _LL20F;_LL224: {struct Cyc_Absyn_Switch_s_struct*_tmp3AC=(struct Cyc_Absyn_Switch_s_struct*)
_tmp390;if(_tmp3AC->tag != 10)goto _LL226;else{_tmp3AD=_tmp3AC->f1;_tmp3AE=_tmp3AC->f2;}}
_LL225: s=({struct Cyc_PP_Doc*_tmp3ED[8];_tmp3ED[7]=Cyc_Absynpp_rb();_tmp3ED[6]=
Cyc_PP_line_doc();_tmp3ED[5]=Cyc_Absynpp_switchclauses2doc(_tmp3AE);_tmp3ED[4]=
Cyc_PP_line_doc();_tmp3ED[3]=Cyc_Absynpp_lb();_tmp3ED[2]=Cyc_PP_text(({const char*
_tmp3EF=") ";_tag_dyneither(_tmp3EF,sizeof(char),3);}));_tmp3ED[1]=Cyc_Absynpp_exp2doc(
_tmp3AD);_tmp3ED[0]=Cyc_PP_text(({const char*_tmp3EE="switch (";_tag_dyneither(
_tmp3EE,sizeof(char),9);}));Cyc_PP_cat(_tag_dyneither(_tmp3ED,sizeof(struct Cyc_PP_Doc*),
8));});goto _LL20F;_LL226: {struct Cyc_Absyn_Fallthru_s_struct*_tmp3AF=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp390;if(_tmp3AF->tag != 11)goto _LL228;else{_tmp3B0=_tmp3AF->f1;if(_tmp3B0 != 0)
goto _LL228;}}_LL227: s=Cyc_PP_text(({const char*_tmp3F0="fallthru;";_tag_dyneither(
_tmp3F0,sizeof(char),10);}));goto _LL20F;_LL228: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp3B1=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp390;if(_tmp3B1->tag != 11)goto
_LL22A;else{_tmp3B2=_tmp3B1->f1;}}_LL229: s=({struct Cyc_PP_Doc*_tmp3F1[3];_tmp3F1[
2]=Cyc_PP_text(({const char*_tmp3F3=");";_tag_dyneither(_tmp3F3,sizeof(char),3);}));
_tmp3F1[1]=Cyc_Absynpp_exps2doc_prec(20,_tmp3B2);_tmp3F1[0]=Cyc_PP_text(({const
char*_tmp3F2="fallthru(";_tag_dyneither(_tmp3F2,sizeof(char),10);}));Cyc_PP_cat(
_tag_dyneither(_tmp3F1,sizeof(struct Cyc_PP_Doc*),3));});goto _LL20F;_LL22A: {
struct Cyc_Absyn_Decl_s_struct*_tmp3B3=(struct Cyc_Absyn_Decl_s_struct*)_tmp390;
if(_tmp3B3->tag != 12)goto _LL22C;else{_tmp3B4=_tmp3B3->f1;_tmp3B5=_tmp3B3->f2;}}
_LL22B: s=({struct Cyc_PP_Doc*_tmp3F4[3];_tmp3F4[2]=Cyc_Absynpp_stmt2doc(_tmp3B5,
stmtexp);_tmp3F4[1]=Cyc_PP_line_doc();_tmp3F4[0]=Cyc_Absynpp_decl2doc(_tmp3B4);
Cyc_PP_cat(_tag_dyneither(_tmp3F4,sizeof(struct Cyc_PP_Doc*),3));});goto _LL20F;
_LL22C: {struct Cyc_Absyn_Label_s_struct*_tmp3B6=(struct Cyc_Absyn_Label_s_struct*)
_tmp390;if(_tmp3B6->tag != 13)goto _LL22E;else{_tmp3B7=_tmp3B6->f1;_tmp3B8=_tmp3B6->f2;}}
_LL22D: if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp3B8)){if(
stmtexp)s=({struct Cyc_PP_Doc*_tmp3F5[8];_tmp3F5[7]=Cyc_PP_text(({const char*
_tmp3F7=");";_tag_dyneither(_tmp3F7,sizeof(char),3);}));_tmp3F5[6]=Cyc_Absynpp_rb();
_tmp3F5[5]=Cyc_PP_line_doc();_tmp3F5[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3B8,1));_tmp3F5[3]=Cyc_PP_line_doc();_tmp3F5[2]=Cyc_Absynpp_lb();_tmp3F5[1]=
Cyc_PP_text(({const char*_tmp3F6=": (";_tag_dyneither(_tmp3F6,sizeof(char),4);}));
_tmp3F5[0]=Cyc_PP_textptr(_tmp3B7);Cyc_PP_cat(_tag_dyneither(_tmp3F5,sizeof(
struct Cyc_PP_Doc*),8));});else{s=({struct Cyc_PP_Doc*_tmp3F8[7];_tmp3F8[6]=Cyc_Absynpp_rb();
_tmp3F8[5]=Cyc_PP_line_doc();_tmp3F8[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3B8,0));_tmp3F8[3]=Cyc_PP_line_doc();_tmp3F8[2]=Cyc_Absynpp_lb();_tmp3F8[1]=
Cyc_PP_text(({const char*_tmp3F9=": ";_tag_dyneither(_tmp3F9,sizeof(char),3);}));
_tmp3F8[0]=Cyc_PP_textptr(_tmp3B7);Cyc_PP_cat(_tag_dyneither(_tmp3F8,sizeof(
struct Cyc_PP_Doc*),7));});}}else{s=({struct Cyc_PP_Doc*_tmp3FA[3];_tmp3FA[2]=Cyc_Absynpp_stmt2doc(
_tmp3B8,stmtexp);_tmp3FA[1]=Cyc_PP_text(({const char*_tmp3FB=": ";_tag_dyneither(
_tmp3FB,sizeof(char),3);}));_tmp3FA[0]=Cyc_PP_textptr(_tmp3B7);Cyc_PP_cat(
_tag_dyneither(_tmp3FA,sizeof(struct Cyc_PP_Doc*),3));});}goto _LL20F;_LL22E: {
struct Cyc_Absyn_Do_s_struct*_tmp3B9=(struct Cyc_Absyn_Do_s_struct*)_tmp390;if(
_tmp3B9->tag != 14)goto _LL230;else{_tmp3BA=_tmp3B9->f1;_tmp3BB=_tmp3B9->f2;
_tmp3BC=_tmp3BB.f1;}}_LL22F: s=({struct Cyc_PP_Doc*_tmp3FC[9];_tmp3FC[8]=Cyc_PP_text(({
const char*_tmp3FF=");";_tag_dyneither(_tmp3FF,sizeof(char),3);}));_tmp3FC[7]=Cyc_Absynpp_exp2doc(
_tmp3BC);_tmp3FC[6]=Cyc_PP_text(({const char*_tmp3FE=" while (";_tag_dyneither(
_tmp3FE,sizeof(char),9);}));_tmp3FC[5]=Cyc_Absynpp_rb();_tmp3FC[4]=Cyc_PP_line_doc();
_tmp3FC[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3BA,0));_tmp3FC[2]=Cyc_PP_line_doc();
_tmp3FC[1]=Cyc_Absynpp_lb();_tmp3FC[0]=Cyc_PP_text(({const char*_tmp3FD="do ";
_tag_dyneither(_tmp3FD,sizeof(char),4);}));Cyc_PP_cat(_tag_dyneither(_tmp3FC,
sizeof(struct Cyc_PP_Doc*),9));});goto _LL20F;_LL230: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp3BD=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp390;if(_tmp3BD->tag != 15)goto
_LL232;else{_tmp3BE=_tmp3BD->f1;_tmp3BF=_tmp3BD->f2;}}_LL231: s=({struct Cyc_PP_Doc*
_tmp400[12];_tmp400[11]=Cyc_Absynpp_rb();_tmp400[10]=Cyc_PP_line_doc();_tmp400[9]=
Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(_tmp3BF));_tmp400[8]=Cyc_PP_line_doc();
_tmp400[7]=Cyc_Absynpp_lb();_tmp400[6]=Cyc_PP_text(({const char*_tmp402=" catch ";
_tag_dyneither(_tmp402,sizeof(char),8);}));_tmp400[5]=Cyc_Absynpp_rb();_tmp400[4]=
Cyc_PP_line_doc();_tmp400[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3BE,0));
_tmp400[2]=Cyc_PP_line_doc();_tmp400[1]=Cyc_Absynpp_lb();_tmp400[0]=Cyc_PP_text(({
const char*_tmp401="try ";_tag_dyneither(_tmp401,sizeof(char),5);}));Cyc_PP_cat(
_tag_dyneither(_tmp400,sizeof(struct Cyc_PP_Doc*),12));});goto _LL20F;_LL232: {
struct Cyc_Absyn_ResetRegion_s_struct*_tmp3C0=(struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp390;if(_tmp3C0->tag != 16)goto _LL20F;else{_tmp3C1=_tmp3C0->f1;}}_LL233: s=({
struct Cyc_PP_Doc*_tmp403[3];_tmp403[2]=Cyc_PP_text(({const char*_tmp405=");";
_tag_dyneither(_tmp405,sizeof(char),3);}));_tmp403[1]=Cyc_Absynpp_exp2doc(
_tmp3C1);_tmp403[0]=Cyc_PP_text(({const char*_tmp404="reset_region(";
_tag_dyneither(_tmp404,sizeof(char),14);}));Cyc_PP_cat(_tag_dyneither(_tmp403,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL20F;_LL20F:;}return s;}struct Cyc_PP_Doc*
Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){struct Cyc_PP_Doc*s;{void*_tmp406=p->r;
enum Cyc_Absyn_Sign _tmp40A;int _tmp40B;char _tmp40D;struct _dyneither_ptr _tmp40F;
struct Cyc_Absyn_Vardecl*_tmp411;struct Cyc_Absyn_Pat*_tmp412;struct Cyc_Absyn_Pat
_tmp413;void*_tmp414;struct Cyc_Absyn_Vardecl*_tmp417;struct Cyc_Absyn_Pat*_tmp418;
struct Cyc_Absyn_Tvar*_tmp41A;struct Cyc_Absyn_Vardecl*_tmp41B;struct Cyc_List_List*
_tmp41D;int _tmp41E;struct Cyc_Absyn_Pat*_tmp420;struct Cyc_Absyn_Vardecl*_tmp422;
struct Cyc_Absyn_Pat*_tmp423;struct Cyc_Absyn_Pat _tmp424;void*_tmp425;struct Cyc_Absyn_Vardecl*
_tmp428;struct Cyc_Absyn_Pat*_tmp429;struct _tuple0*_tmp42B;struct _tuple0*_tmp42D;
struct Cyc_List_List*_tmp42E;int _tmp42F;struct Cyc_Absyn_AggrInfo*_tmp431;struct
Cyc_Absyn_AggrInfo _tmp432;union Cyc_Absyn_AggrInfoU _tmp433;struct Cyc_List_List*
_tmp434;struct Cyc_List_List*_tmp435;int _tmp436;struct Cyc_Absyn_AggrInfo*_tmp438;
struct Cyc_List_List*_tmp439;struct Cyc_List_List*_tmp43A;int _tmp43B;struct Cyc_Absyn_Enumfield*
_tmp43D;struct Cyc_Absyn_Enumfield*_tmp43F;struct Cyc_Absyn_Datatypefield*_tmp441;
struct Cyc_List_List*_tmp442;struct Cyc_Absyn_Datatypefield*_tmp444;struct Cyc_List_List*
_tmp445;int _tmp446;struct Cyc_Absyn_Exp*_tmp448;_LL23A: {struct Cyc_Absyn_Wild_p_struct*
_tmp407=(struct Cyc_Absyn_Wild_p_struct*)_tmp406;if(_tmp407->tag != 0)goto _LL23C;}
_LL23B: s=Cyc_PP_text(({const char*_tmp449="_";_tag_dyneither(_tmp449,sizeof(char),
2);}));goto _LL239;_LL23C: {struct Cyc_Absyn_Null_p_struct*_tmp408=(struct Cyc_Absyn_Null_p_struct*)
_tmp406;if(_tmp408->tag != 8)goto _LL23E;}_LL23D: s=Cyc_PP_text(({const char*_tmp44A="NULL";
_tag_dyneither(_tmp44A,sizeof(char),5);}));goto _LL239;_LL23E: {struct Cyc_Absyn_Int_p_struct*
_tmp409=(struct Cyc_Absyn_Int_p_struct*)_tmp406;if(_tmp409->tag != 9)goto _LL240;
else{_tmp40A=_tmp409->f1;_tmp40B=_tmp409->f2;}}_LL23F: if(_tmp40A != (enum Cyc_Absyn_Sign)
Cyc_Absyn_Unsigned)s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_struct
_tmp44D;_tmp44D.tag=1;_tmp44D.f1=(unsigned long)_tmp40B;({void*_tmp44B[1]={&
_tmp44D};Cyc_aprintf(({const char*_tmp44C="%d";_tag_dyneither(_tmp44C,sizeof(char),
3);}),_tag_dyneither(_tmp44B,sizeof(void*),1));});}));else{s=Cyc_PP_text((struct
_dyneither_ptr)({struct Cyc_Int_pa_struct _tmp450;_tmp450.tag=1;_tmp450.f1=(
unsigned int)_tmp40B;({void*_tmp44E[1]={& _tmp450};Cyc_aprintf(({const char*
_tmp44F="%u";_tag_dyneither(_tmp44F,sizeof(char),3);}),_tag_dyneither(_tmp44E,
sizeof(void*),1));});}));}goto _LL239;_LL240: {struct Cyc_Absyn_Char_p_struct*
_tmp40C=(struct Cyc_Absyn_Char_p_struct*)_tmp406;if(_tmp40C->tag != 10)goto _LL242;
else{_tmp40D=_tmp40C->f1;}}_LL241: s=Cyc_PP_text((struct _dyneither_ptr)({struct
Cyc_String_pa_struct _tmp453;_tmp453.tag=0;_tmp453.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp40D));({void*_tmp451[1]={&
_tmp453};Cyc_aprintf(({const char*_tmp452="'%s'";_tag_dyneither(_tmp452,sizeof(
char),5);}),_tag_dyneither(_tmp451,sizeof(void*),1));});}));goto _LL239;_LL242: {
struct Cyc_Absyn_Float_p_struct*_tmp40E=(struct Cyc_Absyn_Float_p_struct*)_tmp406;
if(_tmp40E->tag != 11)goto _LL244;else{_tmp40F=_tmp40E->f1;}}_LL243: s=Cyc_PP_text(
_tmp40F);goto _LL239;_LL244: {struct Cyc_Absyn_Var_p_struct*_tmp410=(struct Cyc_Absyn_Var_p_struct*)
_tmp406;if(_tmp410->tag != 1)goto _LL246;else{_tmp411=_tmp410->f1;_tmp412=_tmp410->f2;
_tmp413=*_tmp412;_tmp414=_tmp413.r;{struct Cyc_Absyn_Wild_p_struct*_tmp415=(
struct Cyc_Absyn_Wild_p_struct*)_tmp414;if(_tmp415->tag != 0)goto _LL246;};}}_LL245:
s=Cyc_Absynpp_qvar2doc(_tmp411->name);goto _LL239;_LL246: {struct Cyc_Absyn_Var_p_struct*
_tmp416=(struct Cyc_Absyn_Var_p_struct*)_tmp406;if(_tmp416->tag != 1)goto _LL248;
else{_tmp417=_tmp416->f1;_tmp418=_tmp416->f2;}}_LL247: s=({struct Cyc_PP_Doc*
_tmp454[3];_tmp454[2]=Cyc_Absynpp_pat2doc(_tmp418);_tmp454[1]=Cyc_PP_text(({
const char*_tmp455=" as ";_tag_dyneither(_tmp455,sizeof(char),5);}));_tmp454[0]=
Cyc_Absynpp_qvar2doc(_tmp417->name);Cyc_PP_cat(_tag_dyneither(_tmp454,sizeof(
struct Cyc_PP_Doc*),3));});goto _LL239;_LL248: {struct Cyc_Absyn_TagInt_p_struct*
_tmp419=(struct Cyc_Absyn_TagInt_p_struct*)_tmp406;if(_tmp419->tag != 3)goto _LL24A;
else{_tmp41A=_tmp419->f1;_tmp41B=_tmp419->f2;}}_LL249: s=({struct Cyc_PP_Doc*
_tmp456[4];_tmp456[3]=Cyc_PP_text(({const char*_tmp458=">";_tag_dyneither(_tmp458,
sizeof(char),2);}));_tmp456[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp41A));
_tmp456[1]=Cyc_PP_text(({const char*_tmp457="<";_tag_dyneither(_tmp457,sizeof(
char),2);}));_tmp456[0]=Cyc_Absynpp_qvar2doc(_tmp41B->name);Cyc_PP_cat(
_tag_dyneither(_tmp456,sizeof(struct Cyc_PP_Doc*),4));});goto _LL239;_LL24A: {
struct Cyc_Absyn_Tuple_p_struct*_tmp41C=(struct Cyc_Absyn_Tuple_p_struct*)_tmp406;
if(_tmp41C->tag != 4)goto _LL24C;else{_tmp41D=_tmp41C->f1;_tmp41E=_tmp41C->f2;}}
_LL24B: s=({struct Cyc_PP_Doc*_tmp459[4];_tmp459[3]=_tmp41E?Cyc_PP_text(({const
char*_tmp45C=", ...)";_tag_dyneither(_tmp45C,sizeof(char),7);})): Cyc_PP_text(({
const char*_tmp45D=")";_tag_dyneither(_tmp45D,sizeof(char),2);}));_tmp459[2]=((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct
_dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,({
const char*_tmp45B=",";_tag_dyneither(_tmp45B,sizeof(char),2);}),_tmp41D);_tmp459[
1]=Cyc_PP_text(({const char*_tmp45A="(";_tag_dyneither(_tmp45A,sizeof(char),2);}));
_tmp459[0]=Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_dyneither(_tmp459,sizeof(struct
Cyc_PP_Doc*),4));});goto _LL239;_LL24C: {struct Cyc_Absyn_Pointer_p_struct*_tmp41F=(
struct Cyc_Absyn_Pointer_p_struct*)_tmp406;if(_tmp41F->tag != 5)goto _LL24E;else{
_tmp420=_tmp41F->f1;}}_LL24D: s=({struct Cyc_PP_Doc*_tmp45E[2];_tmp45E[1]=Cyc_Absynpp_pat2doc(
_tmp420);_tmp45E[0]=Cyc_PP_text(({const char*_tmp45F="&";_tag_dyneither(_tmp45F,
sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp45E,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL239;_LL24E: {struct Cyc_Absyn_Reference_p_struct*_tmp421=(struct Cyc_Absyn_Reference_p_struct*)
_tmp406;if(_tmp421->tag != 2)goto _LL250;else{_tmp422=_tmp421->f1;_tmp423=_tmp421->f2;
_tmp424=*_tmp423;_tmp425=_tmp424.r;{struct Cyc_Absyn_Wild_p_struct*_tmp426=(
struct Cyc_Absyn_Wild_p_struct*)_tmp425;if(_tmp426->tag != 0)goto _LL250;};}}_LL24F:
s=({struct Cyc_PP_Doc*_tmp460[2];_tmp460[1]=Cyc_Absynpp_qvar2doc(_tmp422->name);
_tmp460[0]=Cyc_PP_text(({const char*_tmp461="*";_tag_dyneither(_tmp461,sizeof(
char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp460,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL239;_LL250: {struct Cyc_Absyn_Reference_p_struct*_tmp427=(struct Cyc_Absyn_Reference_p_struct*)
_tmp406;if(_tmp427->tag != 2)goto _LL252;else{_tmp428=_tmp427->f1;_tmp429=_tmp427->f2;}}
_LL251: s=({struct Cyc_PP_Doc*_tmp462[4];_tmp462[3]=Cyc_Absynpp_pat2doc(_tmp429);
_tmp462[2]=Cyc_PP_text(({const char*_tmp464=" as ";_tag_dyneither(_tmp464,sizeof(
char),5);}));_tmp462[1]=Cyc_Absynpp_qvar2doc(_tmp428->name);_tmp462[0]=Cyc_PP_text(({
const char*_tmp463="*";_tag_dyneither(_tmp463,sizeof(char),2);}));Cyc_PP_cat(
_tag_dyneither(_tmp462,sizeof(struct Cyc_PP_Doc*),4));});goto _LL239;_LL252: {
struct Cyc_Absyn_UnknownId_p_struct*_tmp42A=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp406;if(_tmp42A->tag != 14)goto _LL254;else{_tmp42B=_tmp42A->f1;}}_LL253: s=Cyc_Absynpp_qvar2doc(
_tmp42B);goto _LL239;_LL254: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp42C=(
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp406;if(_tmp42C->tag != 15)goto _LL256;
else{_tmp42D=_tmp42C->f1;_tmp42E=_tmp42C->f2;_tmp42F=_tmp42C->f3;}}_LL255: {
struct _dyneither_ptr term=_tmp42F?({const char*_tmp468=", ...)";_tag_dyneither(
_tmp468,sizeof(char),7);}):({const char*_tmp469=")";_tag_dyneither(_tmp469,
sizeof(char),2);});s=({struct Cyc_PP_Doc*_tmp465[2];_tmp465[1]=Cyc_PP_group(({
const char*_tmp466="(";_tag_dyneither(_tmp466,sizeof(char),2);}),term,({const char*
_tmp467=",";_tag_dyneither(_tmp467,sizeof(char),2);}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_pat2doc,_tmp42E));_tmp465[0]=Cyc_Absynpp_qvar2doc(_tmp42D);Cyc_PP_cat(
_tag_dyneither(_tmp465,sizeof(struct Cyc_PP_Doc*),2));});goto _LL239;}_LL256: {
struct Cyc_Absyn_Aggr_p_struct*_tmp430=(struct Cyc_Absyn_Aggr_p_struct*)_tmp406;
if(_tmp430->tag != 6)goto _LL258;else{_tmp431=_tmp430->f1;if(_tmp431 == 0)goto
_LL258;_tmp432=*_tmp431;_tmp433=_tmp432.aggr_info;_tmp434=_tmp430->f2;_tmp435=
_tmp430->f3;_tmp436=_tmp430->f4;}}_LL257: {struct _dyneither_ptr term=_tmp436?({
const char*_tmp472=", ...}";_tag_dyneither(_tmp472,sizeof(char),7);}):({const char*
_tmp473="}";_tag_dyneither(_tmp473,sizeof(char),2);});struct _tuple0*_tmp46B;
struct _tuple9 _tmp46A=Cyc_Absyn_aggr_kinded_name(_tmp433);_tmp46B=_tmp46A.f2;s=({
struct Cyc_PP_Doc*_tmp46C[4];_tmp46C[3]=Cyc_PP_group(({const char*_tmp470="";
_tag_dyneither(_tmp470,sizeof(char),1);}),term,({const char*_tmp471=",";
_tag_dyneither(_tmp471,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp435));_tmp46C[2]=Cyc_PP_egroup(({const char*_tmp46D="[";_tag_dyneither(
_tmp46D,sizeof(char),2);}),({const char*_tmp46E="]";_tag_dyneither(_tmp46E,
sizeof(char),2);}),({const char*_tmp46F=",";_tag_dyneither(_tmp46F,sizeof(char),2);}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,
_tmp434)));_tmp46C[1]=Cyc_Absynpp_lb();_tmp46C[0]=Cyc_Absynpp_qvar2doc(_tmp46B);
Cyc_PP_cat(_tag_dyneither(_tmp46C,sizeof(struct Cyc_PP_Doc*),4));});goto _LL239;}
_LL258: {struct Cyc_Absyn_Aggr_p_struct*_tmp437=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp406;if(_tmp437->tag != 6)goto _LL25A;else{_tmp438=_tmp437->f1;if(_tmp438 != 0)
goto _LL25A;_tmp439=_tmp437->f2;_tmp43A=_tmp437->f3;_tmp43B=_tmp437->f4;}}_LL259: {
struct _dyneither_ptr term=_tmp43B?({const char*_tmp47A=", ...}";_tag_dyneither(
_tmp47A,sizeof(char),7);}):({const char*_tmp47B="}";_tag_dyneither(_tmp47B,
sizeof(char),2);});s=({struct Cyc_PP_Doc*_tmp474[3];_tmp474[2]=Cyc_PP_group(({
const char*_tmp478="";_tag_dyneither(_tmp478,sizeof(char),1);}),term,({const char*
_tmp479=",";_tag_dyneither(_tmp479,sizeof(char),2);}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp43A));_tmp474[1]=Cyc_PP_egroup(({const char*_tmp475="[";_tag_dyneither(
_tmp475,sizeof(char),2);}),({const char*_tmp476="]";_tag_dyneither(_tmp476,
sizeof(char),2);}),({const char*_tmp477=",";_tag_dyneither(_tmp477,sizeof(char),2);}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,
_tmp439)));_tmp474[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_dyneither(_tmp474,sizeof(
struct Cyc_PP_Doc*),3));});goto _LL239;}_LL25A: {struct Cyc_Absyn_Enum_p_struct*
_tmp43C=(struct Cyc_Absyn_Enum_p_struct*)_tmp406;if(_tmp43C->tag != 12)goto _LL25C;
else{_tmp43D=_tmp43C->f2;}}_LL25B: _tmp43F=_tmp43D;goto _LL25D;_LL25C: {struct Cyc_Absyn_AnonEnum_p_struct*
_tmp43E=(struct Cyc_Absyn_AnonEnum_p_struct*)_tmp406;if(_tmp43E->tag != 13)goto
_LL25E;else{_tmp43F=_tmp43E->f2;}}_LL25D: s=Cyc_Absynpp_qvar2doc(_tmp43F->name);
goto _LL239;_LL25E: {struct Cyc_Absyn_Datatype_p_struct*_tmp440=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp406;if(_tmp440->tag != 7)goto _LL260;else{_tmp441=_tmp440->f2;_tmp442=_tmp440->f3;
if(_tmp442 != 0)goto _LL260;}}_LL25F: s=Cyc_Absynpp_qvar2doc(_tmp441->name);goto
_LL239;_LL260: {struct Cyc_Absyn_Datatype_p_struct*_tmp443=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp406;if(_tmp443->tag != 7)goto _LL262;else{_tmp444=_tmp443->f2;_tmp445=_tmp443->f3;
_tmp446=_tmp443->f4;}}_LL261: {struct _dyneither_ptr term=_tmp446?({const char*
_tmp47F=", ...)";_tag_dyneither(_tmp47F,sizeof(char),7);}):({const char*_tmp480=")";
_tag_dyneither(_tmp480,sizeof(char),2);});s=({struct Cyc_PP_Doc*_tmp47C[2];
_tmp47C[1]=Cyc_PP_egroup(({const char*_tmp47D="(";_tag_dyneither(_tmp47D,sizeof(
char),2);}),term,({const char*_tmp47E=",";_tag_dyneither(_tmp47E,sizeof(char),2);}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp445));_tmp47C[0]=Cyc_Absynpp_qvar2doc(
_tmp444->name);Cyc_PP_cat(_tag_dyneither(_tmp47C,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL239;}_LL262: {struct Cyc_Absyn_Exp_p_struct*_tmp447=(struct Cyc_Absyn_Exp_p_struct*)
_tmp406;if(_tmp447->tag != 16)goto _LL239;else{_tmp448=_tmp447->f1;}}_LL263: s=Cyc_Absynpp_exp2doc(
_tmp448);goto _LL239;_LL239:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct
_tuple13*dp){return({struct Cyc_PP_Doc*_tmp481[2];_tmp481[1]=Cyc_Absynpp_pat2doc((*
dp).f2);_tmp481[0]=Cyc_PP_egroup(({const char*_tmp482="";_tag_dyneither(_tmp482,
sizeof(char),1);}),({const char*_tmp483="=";_tag_dyneither(_tmp483,sizeof(char),2);}),({
const char*_tmp484="=";_tag_dyneither(_tmp484,sizeof(char),2);}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*
dp).f1));Cyc_PP_cat(_tag_dyneither(_tmp481,sizeof(struct Cyc_PP_Doc*),2));});}
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
if(c->where_clause == 0  && (c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val)return({
struct Cyc_PP_Doc*_tmp485[2];_tmp485[1]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp487[
2];_tmp487[1]=Cyc_Absynpp_stmt2doc(c->body,0);_tmp487[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_dyneither(_tmp487,sizeof(struct Cyc_PP_Doc*),2));}));_tmp485[0]=Cyc_PP_text(({
const char*_tmp486="default: ";_tag_dyneither(_tmp486,sizeof(char),10);}));Cyc_PP_cat(
_tag_dyneither(_tmp485,sizeof(struct Cyc_PP_Doc*),2));});else{if(c->where_clause
== 0)return({struct Cyc_PP_Doc*_tmp488[4];_tmp488[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*
_tmp48B[2];_tmp48B[1]=Cyc_Absynpp_stmt2doc(c->body,0);_tmp48B[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_dyneither(_tmp48B,sizeof(struct Cyc_PP_Doc*),2));}));_tmp488[2]=
Cyc_PP_text(({const char*_tmp48A=": ";_tag_dyneither(_tmp48A,sizeof(char),3);}));
_tmp488[1]=Cyc_Absynpp_pat2doc(c->pattern);_tmp488[0]=Cyc_PP_text(({const char*
_tmp489="case ";_tag_dyneither(_tmp489,sizeof(char),6);}));Cyc_PP_cat(
_tag_dyneither(_tmp488,sizeof(struct Cyc_PP_Doc*),4));});else{return({struct Cyc_PP_Doc*
_tmp48C[6];_tmp48C[5]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp490[2];_tmp490[1]=Cyc_Absynpp_stmt2doc(
c->body,0);_tmp490[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp490,
sizeof(struct Cyc_PP_Doc*),2));}));_tmp48C[4]=Cyc_PP_text(({const char*_tmp48F=": ";
_tag_dyneither(_tmp48F,sizeof(char),3);}));_tmp48C[3]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(c->where_clause));_tmp48C[2]=Cyc_PP_text(({
const char*_tmp48E=" && ";_tag_dyneither(_tmp48E,sizeof(char),5);}));_tmp48C[1]=
Cyc_Absynpp_pat2doc(c->pattern);_tmp48C[0]=Cyc_PP_text(({const char*_tmp48D="case ";
_tag_dyneither(_tmp48D,sizeof(char),6);}));Cyc_PP_cat(_tag_dyneither(_tmp48C,
sizeof(struct Cyc_PP_Doc*),6));});}}}struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List*cs){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Switch_clause*),struct _dyneither_ptr sep,struct Cyc_List_List*l))
Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,({const char*_tmp491="";
_tag_dyneither(_tmp491,sizeof(char),1);}),cs);}struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(
struct Cyc_Absyn_Enumfield*f){if(f->tag == 0)return Cyc_Absynpp_qvar2doc(f->name);
else{return({struct Cyc_PP_Doc*_tmp492[3];_tmp492[2]=Cyc_Absynpp_exp2doc((struct
Cyc_Absyn_Exp*)_check_null(f->tag));_tmp492[1]=Cyc_PP_text(({const char*_tmp493=" = ";
_tag_dyneither(_tmp493,sizeof(char),4);}));_tmp492[0]=Cyc_Absynpp_qvar2doc(f->name);
Cyc_PP_cat(_tag_dyneither(_tmp492,sizeof(struct Cyc_PP_Doc*),3));});}}struct Cyc_PP_Doc*
Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,
struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,({const char*
_tmp494=",";_tag_dyneither(_tmp494,sizeof(char),2);}),fs);}static struct Cyc_PP_Doc*
Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){return Cyc_Absynpp_qvar2doc(v->name);}
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct
_dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,({
const char*_tmp495=",";_tag_dyneither(_tmp495,sizeof(char),2);}),vds);}struct Cyc_PP_Doc*
Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){struct Cyc_Absyn_Vardecl
_tmp497;enum Cyc_Absyn_Scope _tmp498;struct _tuple0*_tmp499;struct Cyc_Absyn_Tqual
_tmp49A;void*_tmp49B;struct Cyc_Absyn_Exp*_tmp49C;struct Cyc_List_List*_tmp49D;
struct Cyc_Absyn_Vardecl*_tmp496=vd;_tmp497=*_tmp496;_tmp498=_tmp497.sc;_tmp499=
_tmp497.name;_tmp49A=_tmp497.tq;_tmp49B=_tmp497.type;_tmp49C=_tmp497.initializer;
_tmp49D=_tmp497.attributes;{struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(
_tmp499);struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp49D);struct Cyc_PP_Doc*
beforenamedoc;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL264:
beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL265:{void*_tmp49E=Cyc_Tcutil_compress(
_tmp49B);struct Cyc_Absyn_FnInfo _tmp4A0;struct Cyc_List_List*_tmp4A1;_LL268: {
struct Cyc_Absyn_FnType_struct*_tmp49F=(struct Cyc_Absyn_FnType_struct*)_tmp49E;
if(_tmp49F->tag != 10)goto _LL26A;else{_tmp4A0=_tmp49F->f1;_tmp4A1=_tmp4A0.attributes;}}
_LL269: beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp4A1);goto _LL267;_LL26A:;_LL26B:
beforenamedoc=Cyc_PP_nil_doc();goto _LL267;_LL267:;}break;}{struct Cyc_PP_Doc*
tmp_doc;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL26C: tmp_doc=Cyc_PP_nil_doc();
break;case Cyc_Cyclone_Vc_c: _LL26D: tmp_doc=attsdoc;break;}s=({struct Cyc_PP_Doc*
_tmp4A2[5];_tmp4A2[4]=Cyc_PP_text(({const char*_tmp4A7=";";_tag_dyneither(_tmp4A7,
sizeof(char),2);}));_tmp4A2[3]=_tmp49C == 0?Cyc_PP_nil_doc():({struct Cyc_PP_Doc*
_tmp4A5[2];_tmp4A5[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp49C);_tmp4A5[
0]=Cyc_PP_text(({const char*_tmp4A6=" = ";_tag_dyneither(_tmp4A6,sizeof(char),4);}));
Cyc_PP_cat(_tag_dyneither(_tmp4A5,sizeof(struct Cyc_PP_Doc*),2));});_tmp4A2[2]=
Cyc_Absynpp_tqtd2doc(_tmp49A,_tmp49B,({struct Cyc_Core_Opt*_tmp4A3=_cycalloc(
sizeof(*_tmp4A3));_tmp4A3->v=({struct Cyc_PP_Doc*_tmp4A4[2];_tmp4A4[1]=sn;_tmp4A4[
0]=beforenamedoc;Cyc_PP_cat(_tag_dyneither(_tmp4A4,sizeof(struct Cyc_PP_Doc*),2));});
_tmp4A3;}));_tmp4A2[1]=Cyc_Absynpp_scope2doc(_tmp498);_tmp4A2[0]=tmp_doc;Cyc_PP_cat(
_tag_dyneither(_tmp4A2,sizeof(struct Cyc_PP_Doc*),5));});return s;};};}struct
_tuple17{struct Cyc_Position_Segment*f1;struct _tuple0*f2;int f3;};struct Cyc_PP_Doc*
Cyc_Absynpp_export2doc(struct _tuple17*x){struct _tuple0*_tmp4A9;struct _tuple17
_tmp4A8=*x;_tmp4A9=_tmp4A8.f2;return Cyc_Absynpp_qvar2doc(_tmp4A9);}struct Cyc_PP_Doc*
Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){struct Cyc_PP_Doc*s;{void*_tmp4AA=d->r;
struct Cyc_Absyn_Fndecl*_tmp4AC;struct Cyc_Absyn_Aggrdecl*_tmp4AE;struct Cyc_Absyn_Vardecl*
_tmp4B0;struct Cyc_Absyn_Tvar*_tmp4B2;struct Cyc_Absyn_Vardecl*_tmp4B3;int _tmp4B4;
struct Cyc_Absyn_Exp*_tmp4B5;struct Cyc_Absyn_Exp*_tmp4B7;struct Cyc_Absyn_Tvar*
_tmp4B8;struct Cyc_Absyn_Vardecl*_tmp4B9;struct Cyc_Absyn_Datatypedecl*_tmp4BB;
struct Cyc_Absyn_Datatypedecl _tmp4BC;enum Cyc_Absyn_Scope _tmp4BD;struct _tuple0*
_tmp4BE;struct Cyc_List_List*_tmp4BF;struct Cyc_Core_Opt*_tmp4C0;int _tmp4C1;struct
Cyc_Absyn_Pat*_tmp4C3;struct Cyc_Absyn_Exp*_tmp4C4;struct Cyc_List_List*_tmp4C6;
struct Cyc_Absyn_Enumdecl*_tmp4C8;struct Cyc_Absyn_Enumdecl _tmp4C9;enum Cyc_Absyn_Scope
_tmp4CA;struct _tuple0*_tmp4CB;struct Cyc_Core_Opt*_tmp4CC;struct Cyc_Absyn_Typedefdecl*
_tmp4CE;struct _dyneither_ptr*_tmp4D0;struct Cyc_List_List*_tmp4D1;struct _tuple0*
_tmp4D3;struct Cyc_List_List*_tmp4D4;struct Cyc_List_List*_tmp4D6;struct Cyc_List_List*
_tmp4D8;struct Cyc_List_List*_tmp4D9;_LL270: {struct Cyc_Absyn_Fn_d_struct*_tmp4AB=(
struct Cyc_Absyn_Fn_d_struct*)_tmp4AA;if(_tmp4AB->tag != 1)goto _LL272;else{_tmp4AC=
_tmp4AB->f1;}}_LL271: {void*t=(void*)({struct Cyc_Absyn_FnType_struct*_tmp4E4=
_cycalloc(sizeof(*_tmp4E4));_tmp4E4[0]=({struct Cyc_Absyn_FnType_struct _tmp4E5;
_tmp4E5.tag=10;_tmp4E5.f1=({struct Cyc_Absyn_FnInfo _tmp4E6;_tmp4E6.tvars=_tmp4AC->tvs;
_tmp4E6.effect=_tmp4AC->effect;_tmp4E6.ret_typ=_tmp4AC->ret_type;_tmp4E6.args=((
struct Cyc_List_List*(*)(struct _tuple7*(*f)(struct _tuple11*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp4AC->args);_tmp4E6.c_varargs=_tmp4AC->c_varargs;
_tmp4E6.cyc_varargs=_tmp4AC->cyc_varargs;_tmp4E6.rgn_po=_tmp4AC->rgn_po;_tmp4E6.attributes=
0;_tmp4E6;});_tmp4E5;});_tmp4E4;});struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(
_tmp4AC->attributes);struct Cyc_PP_Doc*inlinedoc;if(_tmp4AC->is_inline)switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LL290: inlinedoc=Cyc_PP_text(({const char*_tmp4DC="inline ";
_tag_dyneither(_tmp4DC,sizeof(char),8);}));break;case Cyc_Cyclone_Vc_c: _LL291:
inlinedoc=Cyc_PP_text(({const char*_tmp4DD="__inline ";_tag_dyneither(_tmp4DD,
sizeof(char),10);}));break;}else{inlinedoc=Cyc_PP_nil_doc();}{struct Cyc_PP_Doc*
scopedoc=Cyc_Absynpp_scope2doc(_tmp4AC->sc);struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL293: beforenamedoc=
attsdoc;break;case Cyc_Cyclone_Vc_c: _LL294: beforenamedoc=Cyc_Absynpp_callconv2doc(
_tmp4AC->attributes);break;}{struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(
_tmp4AC->name);struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(
0),t,({struct Cyc_Core_Opt*_tmp4E2=_cycalloc(sizeof(*_tmp4E2));_tmp4E2->v=({
struct Cyc_PP_Doc*_tmp4E3[2];_tmp4E3[1]=namedoc;_tmp4E3[0]=beforenamedoc;Cyc_PP_cat(
_tag_dyneither(_tmp4E3,sizeof(struct Cyc_PP_Doc*),2));});_tmp4E2;}));struct Cyc_PP_Doc*
bodydoc=({struct Cyc_PP_Doc*_tmp4E0[5];_tmp4E0[4]=Cyc_Absynpp_rb();_tmp4E0[3]=Cyc_PP_line_doc();
_tmp4E0[2]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp4E1[2];_tmp4E1[1]=Cyc_Absynpp_stmt2doc(
_tmp4AC->body,0);_tmp4E1[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp4E1,
sizeof(struct Cyc_PP_Doc*),2));}));_tmp4E0[1]=Cyc_Absynpp_lb();_tmp4E0[0]=Cyc_PP_blank_doc();
Cyc_PP_cat(_tag_dyneither(_tmp4E0,sizeof(struct Cyc_PP_Doc*),5));});s=({struct Cyc_PP_Doc*
_tmp4DE[4];_tmp4DE[3]=bodydoc;_tmp4DE[2]=tqtddoc;_tmp4DE[1]=scopedoc;_tmp4DE[0]=
inlinedoc;Cyc_PP_cat(_tag_dyneither(_tmp4DE,sizeof(struct Cyc_PP_Doc*),4));});
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL296: s=({struct Cyc_PP_Doc*
_tmp4DF[2];_tmp4DF[1]=s;_tmp4DF[0]=attsdoc;Cyc_PP_cat(_tag_dyneither(_tmp4DF,
sizeof(struct Cyc_PP_Doc*),2));});break;default: _LL297: break;}goto _LL26F;};};}
_LL272: {struct Cyc_Absyn_Aggr_d_struct*_tmp4AD=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp4AA;if(_tmp4AD->tag != 6)goto _LL274;else{_tmp4AE=_tmp4AD->f1;}}_LL273: if(
_tmp4AE->impl == 0)s=({struct Cyc_PP_Doc*_tmp4E7[5];_tmp4E7[4]=Cyc_PP_text(({const
char*_tmp4E8=";";_tag_dyneither(_tmp4E8,sizeof(char),2);}));_tmp4E7[3]=Cyc_Absynpp_ktvars2doc(
_tmp4AE->tvs);_tmp4E7[2]=Cyc_Absynpp_qvar2bolddoc(_tmp4AE->name);_tmp4E7[1]=Cyc_Absynpp_aggr_kind2doc(
_tmp4AE->kind);_tmp4E7[0]=Cyc_Absynpp_scope2doc(_tmp4AE->sc);Cyc_PP_cat(
_tag_dyneither(_tmp4E7,sizeof(struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*
_tmp4E9[13];_tmp4E9[12]=Cyc_PP_text(({const char*_tmp4ED=";";_tag_dyneither(
_tmp4ED,sizeof(char),2);}));_tmp4E9[11]=Cyc_Absynpp_atts2doc(_tmp4AE->attributes);
_tmp4E9[10]=Cyc_Absynpp_rb();_tmp4E9[9]=Cyc_PP_line_doc();_tmp4E9[8]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp4EC[2];_tmp4EC[1]=Cyc_Absynpp_aggrfields2doc(((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4AE->impl))->fields);_tmp4EC[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_dyneither(_tmp4EC,sizeof(struct Cyc_PP_Doc*),2));}));_tmp4E9[7]=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4AE->impl))->rgn_po == 0?Cyc_PP_nil_doc():({
struct Cyc_PP_Doc*_tmp4EA[2];_tmp4EA[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4AE->impl))->rgn_po);_tmp4EA[0]=Cyc_PP_text(({const char*_tmp4EB=":";
_tag_dyneither(_tmp4EB,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp4EA,
sizeof(struct Cyc_PP_Doc*),2));});_tmp4E9[6]=Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4AE->impl))->exist_vars);_tmp4E9[5]=Cyc_Absynpp_lb();_tmp4E9[4]=
Cyc_PP_blank_doc();_tmp4E9[3]=Cyc_Absynpp_ktvars2doc(_tmp4AE->tvs);_tmp4E9[2]=
Cyc_Absynpp_qvar2bolddoc(_tmp4AE->name);_tmp4E9[1]=Cyc_Absynpp_aggr_kind2doc(
_tmp4AE->kind);_tmp4E9[0]=Cyc_Absynpp_scope2doc(_tmp4AE->sc);Cyc_PP_cat(
_tag_dyneither(_tmp4E9,sizeof(struct Cyc_PP_Doc*),13));});}goto _LL26F;_LL274: {
struct Cyc_Absyn_Var_d_struct*_tmp4AF=(struct Cyc_Absyn_Var_d_struct*)_tmp4AA;if(
_tmp4AF->tag != 0)goto _LL276;else{_tmp4B0=_tmp4AF->f1;}}_LL275: s=Cyc_Absynpp_vardecl2doc(
_tmp4B0);goto _LL26F;_LL276: {struct Cyc_Absyn_Region_d_struct*_tmp4B1=(struct Cyc_Absyn_Region_d_struct*)
_tmp4AA;if(_tmp4B1->tag != 4)goto _LL278;else{_tmp4B2=_tmp4B1->f1;_tmp4B3=_tmp4B1->f2;
_tmp4B4=_tmp4B1->f3;_tmp4B5=_tmp4B1->f4;}}_LL277: s=({struct Cyc_PP_Doc*_tmp4EE[8];
_tmp4EE[7]=Cyc_PP_text(({const char*_tmp4F6=";";_tag_dyneither(_tmp4F6,sizeof(
char),2);}));_tmp4EE[6]=_tmp4B5 != 0?({struct Cyc_PP_Doc*_tmp4F3[3];_tmp4F3[2]=Cyc_PP_text(({
const char*_tmp4F5=")";_tag_dyneither(_tmp4F5,sizeof(char),2);}));_tmp4F3[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp4B5);_tmp4F3[0]=Cyc_PP_text(({const char*_tmp4F4=" = open(";
_tag_dyneither(_tmp4F4,sizeof(char),9);}));Cyc_PP_cat(_tag_dyneither(_tmp4F3,
sizeof(struct Cyc_PP_Doc*),3));}): Cyc_PP_nil_doc();_tmp4EE[5]=_tmp4B4?Cyc_PP_text(({
const char*_tmp4F2=" @resetable";_tag_dyneither(_tmp4F2,sizeof(char),12);})): Cyc_PP_nil_doc();
_tmp4EE[4]=Cyc_Absynpp_qvar2doc(_tmp4B3->name);_tmp4EE[3]=Cyc_PP_text(({const
char*_tmp4F1=">";_tag_dyneither(_tmp4F1,sizeof(char),2);}));_tmp4EE[2]=Cyc_PP_textptr(
Cyc_Absynpp_get_name(_tmp4B2));_tmp4EE[1]=Cyc_PP_text(({const char*_tmp4F0="<";
_tag_dyneither(_tmp4F0,sizeof(char),2);}));_tmp4EE[0]=Cyc_PP_text(({const char*
_tmp4EF="region";_tag_dyneither(_tmp4EF,sizeof(char),7);}));Cyc_PP_cat(
_tag_dyneither(_tmp4EE,sizeof(struct Cyc_PP_Doc*),8));});goto _LL26F;_LL278: {
struct Cyc_Absyn_Alias_d_struct*_tmp4B6=(struct Cyc_Absyn_Alias_d_struct*)_tmp4AA;
if(_tmp4B6->tag != 5)goto _LL27A;else{_tmp4B7=_tmp4B6->f1;_tmp4B8=_tmp4B6->f2;
_tmp4B9=_tmp4B6->f3;}}_LL279: s=({struct Cyc_PP_Doc*_tmp4F7[8];_tmp4F7[7]=Cyc_PP_text(({
const char*_tmp4FC=";";_tag_dyneither(_tmp4FC,sizeof(char),2);}));_tmp4F7[6]=Cyc_Absynpp_exp2doc(
_tmp4B7);_tmp4F7[5]=Cyc_PP_text(({const char*_tmp4FB=" = ";_tag_dyneither(_tmp4FB,
sizeof(char),4);}));_tmp4F7[4]=Cyc_Absynpp_qvar2doc(_tmp4B9->name);_tmp4F7[3]=
Cyc_PP_text(({const char*_tmp4FA="> ";_tag_dyneither(_tmp4FA,sizeof(char),3);}));
_tmp4F7[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp4B8));_tmp4F7[1]=Cyc_PP_text(({
const char*_tmp4F9=" <";_tag_dyneither(_tmp4F9,sizeof(char),3);}));_tmp4F7[0]=Cyc_PP_text(({
const char*_tmp4F8="alias";_tag_dyneither(_tmp4F8,sizeof(char),6);}));Cyc_PP_cat(
_tag_dyneither(_tmp4F7,sizeof(struct Cyc_PP_Doc*),8));});goto _LL26F;_LL27A: {
struct Cyc_Absyn_Datatype_d_struct*_tmp4BA=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp4AA;if(_tmp4BA->tag != 7)goto _LL27C;else{_tmp4BB=_tmp4BA->f1;_tmp4BC=*_tmp4BB;
_tmp4BD=_tmp4BC.sc;_tmp4BE=_tmp4BC.name;_tmp4BF=_tmp4BC.tvs;_tmp4C0=_tmp4BC.fields;
_tmp4C1=_tmp4BC.is_extensible;}}_LL27B: if(_tmp4C0 == 0)s=({struct Cyc_PP_Doc*
_tmp4FD[6];_tmp4FD[5]=Cyc_PP_text(({const char*_tmp500=";";_tag_dyneither(_tmp500,
sizeof(char),2);}));_tmp4FD[4]=Cyc_Absynpp_ktvars2doc(_tmp4BF);_tmp4FD[3]=
_tmp4C1?Cyc_Absynpp_qvar2bolddoc(_tmp4BE): Cyc_Absynpp_typedef_name2bolddoc(
_tmp4BE);_tmp4FD[2]=Cyc_PP_text(({const char*_tmp4FF="datatype ";_tag_dyneither(
_tmp4FF,sizeof(char),10);}));_tmp4FD[1]=_tmp4C1?Cyc_PP_text(({const char*_tmp4FE="@extensible";
_tag_dyneither(_tmp4FE,sizeof(char),12);})): Cyc_PP_blank_doc();_tmp4FD[0]=Cyc_Absynpp_scope2doc(
_tmp4BD);Cyc_PP_cat(_tag_dyneither(_tmp4FD,sizeof(struct Cyc_PP_Doc*),6));});
else{s=({struct Cyc_PP_Doc*_tmp501[11];_tmp501[10]=Cyc_PP_text(({const char*
_tmp505=";";_tag_dyneither(_tmp505,sizeof(char),2);}));_tmp501[9]=Cyc_Absynpp_rb();
_tmp501[8]=Cyc_PP_line_doc();_tmp501[7]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp504[
2];_tmp504[1]=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)_tmp4C0->v);
_tmp504[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp504,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp501[6]=Cyc_Absynpp_lb();_tmp501[5]=Cyc_PP_blank_doc();_tmp501[4]=Cyc_Absynpp_ktvars2doc(
_tmp4BF);_tmp501[3]=_tmp4C1?Cyc_Absynpp_qvar2bolddoc(_tmp4BE): Cyc_Absynpp_typedef_name2bolddoc(
_tmp4BE);_tmp501[2]=Cyc_PP_text(({const char*_tmp503="datatype ";_tag_dyneither(
_tmp503,sizeof(char),10);}));_tmp501[1]=_tmp4C1?Cyc_PP_text(({const char*_tmp502="@extensible ";
_tag_dyneither(_tmp502,sizeof(char),13);})): Cyc_PP_blank_doc();_tmp501[0]=Cyc_Absynpp_scope2doc(
_tmp4BD);Cyc_PP_cat(_tag_dyneither(_tmp501,sizeof(struct Cyc_PP_Doc*),11));});}
goto _LL26F;_LL27C: {struct Cyc_Absyn_Let_d_struct*_tmp4C2=(struct Cyc_Absyn_Let_d_struct*)
_tmp4AA;if(_tmp4C2->tag != 2)goto _LL27E;else{_tmp4C3=_tmp4C2->f1;_tmp4C4=_tmp4C2->f3;}}
_LL27D: s=({struct Cyc_PP_Doc*_tmp506[5];_tmp506[4]=Cyc_PP_text(({const char*
_tmp509=";";_tag_dyneither(_tmp509,sizeof(char),2);}));_tmp506[3]=Cyc_Absynpp_exp2doc(
_tmp4C4);_tmp506[2]=Cyc_PP_text(({const char*_tmp508=" = ";_tag_dyneither(_tmp508,
sizeof(char),4);}));_tmp506[1]=Cyc_Absynpp_pat2doc(_tmp4C3);_tmp506[0]=Cyc_PP_text(({
const char*_tmp507="let ";_tag_dyneither(_tmp507,sizeof(char),5);}));Cyc_PP_cat(
_tag_dyneither(_tmp506,sizeof(struct Cyc_PP_Doc*),5));});goto _LL26F;_LL27E: {
struct Cyc_Absyn_Letv_d_struct*_tmp4C5=(struct Cyc_Absyn_Letv_d_struct*)_tmp4AA;
if(_tmp4C5->tag != 3)goto _LL280;else{_tmp4C6=_tmp4C5->f1;}}_LL27F: s=({struct Cyc_PP_Doc*
_tmp50A[3];_tmp50A[2]=Cyc_PP_text(({const char*_tmp50C=";";_tag_dyneither(_tmp50C,
sizeof(char),2);}));_tmp50A[1]=Cyc_Absynpp_ids2doc(_tmp4C6);_tmp50A[0]=Cyc_PP_text(({
const char*_tmp50B="let ";_tag_dyneither(_tmp50B,sizeof(char),5);}));Cyc_PP_cat(
_tag_dyneither(_tmp50A,sizeof(struct Cyc_PP_Doc*),3));});goto _LL26F;_LL280: {
struct Cyc_Absyn_Enum_d_struct*_tmp4C7=(struct Cyc_Absyn_Enum_d_struct*)_tmp4AA;
if(_tmp4C7->tag != 8)goto _LL282;else{_tmp4C8=_tmp4C7->f1;_tmp4C9=*_tmp4C8;_tmp4CA=
_tmp4C9.sc;_tmp4CB=_tmp4C9.name;_tmp4CC=_tmp4C9.fields;}}_LL281: if(_tmp4CC == 0)s=({
struct Cyc_PP_Doc*_tmp50D[4];_tmp50D[3]=Cyc_PP_text(({const char*_tmp50F=";";
_tag_dyneither(_tmp50F,sizeof(char),2);}));_tmp50D[2]=Cyc_Absynpp_typedef_name2bolddoc(
_tmp4CB);_tmp50D[1]=Cyc_PP_text(({const char*_tmp50E="enum ";_tag_dyneither(
_tmp50E,sizeof(char),6);}));_tmp50D[0]=Cyc_Absynpp_scope2doc(_tmp4CA);Cyc_PP_cat(
_tag_dyneither(_tmp50D,sizeof(struct Cyc_PP_Doc*),4));});else{s=({struct Cyc_PP_Doc*
_tmp510[9];_tmp510[8]=Cyc_PP_text(({const char*_tmp513=";";_tag_dyneither(_tmp513,
sizeof(char),2);}));_tmp510[7]=Cyc_Absynpp_rb();_tmp510[6]=Cyc_PP_line_doc();
_tmp510[5]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp512[2];_tmp512[1]=Cyc_Absynpp_enumfields2doc((
struct Cyc_List_List*)_tmp4CC->v);_tmp512[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_dyneither(_tmp512,sizeof(struct Cyc_PP_Doc*),2));}));_tmp510[4]=Cyc_Absynpp_lb();
_tmp510[3]=Cyc_PP_blank_doc();_tmp510[2]=Cyc_Absynpp_qvar2bolddoc(_tmp4CB);
_tmp510[1]=Cyc_PP_text(({const char*_tmp511="enum ";_tag_dyneither(_tmp511,
sizeof(char),6);}));_tmp510[0]=Cyc_Absynpp_scope2doc(_tmp4CA);Cyc_PP_cat(
_tag_dyneither(_tmp510,sizeof(struct Cyc_PP_Doc*),9));});}goto _LL26F;_LL282: {
struct Cyc_Absyn_Typedef_d_struct*_tmp4CD=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp4AA;if(_tmp4CD->tag != 9)goto _LL284;else{_tmp4CE=_tmp4CD->f1;}}_LL283: {void*
t;if(_tmp4CE->defn != 0)t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4CE->defn))->v;
else{t=Cyc_Absyn_new_evar(_tmp4CE->kind,0);}s=({struct Cyc_PP_Doc*_tmp514[4];
_tmp514[3]=Cyc_PP_text(({const char*_tmp518=";";_tag_dyneither(_tmp518,sizeof(
char),2);}));_tmp514[2]=Cyc_Absynpp_atts2doc(_tmp4CE->atts);_tmp514[1]=Cyc_Absynpp_tqtd2doc(
_tmp4CE->tq,t,({struct Cyc_Core_Opt*_tmp516=_cycalloc(sizeof(*_tmp516));_tmp516->v=({
struct Cyc_PP_Doc*_tmp517[2];_tmp517[1]=Cyc_Absynpp_tvars2doc(_tmp4CE->tvs);
_tmp517[0]=Cyc_Absynpp_typedef_name2bolddoc(_tmp4CE->name);Cyc_PP_cat(
_tag_dyneither(_tmp517,sizeof(struct Cyc_PP_Doc*),2));});_tmp516;}));_tmp514[0]=
Cyc_PP_text(({const char*_tmp515="typedef ";_tag_dyneither(_tmp515,sizeof(char),9);}));
Cyc_PP_cat(_tag_dyneither(_tmp514,sizeof(struct Cyc_PP_Doc*),4));});goto _LL26F;}
_LL284: {struct Cyc_Absyn_Namespace_d_struct*_tmp4CF=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp4AA;if(_tmp4CF->tag != 10)goto _LL286;else{_tmp4D0=_tmp4CF->f1;_tmp4D1=_tmp4CF->f2;}}
_LL285: if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp4D0);
s=({struct Cyc_PP_Doc*_tmp519[8];_tmp519[7]=Cyc_Absynpp_rb();_tmp519[6]=Cyc_PP_line_doc();
_tmp519[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp51B="";_tag_dyneither(_tmp51B,sizeof(char),1);}),_tmp4D1);_tmp519[
4]=Cyc_PP_line_doc();_tmp519[3]=Cyc_Absynpp_lb();_tmp519[2]=Cyc_PP_blank_doc();
_tmp519[1]=Cyc_PP_textptr(_tmp4D0);_tmp519[0]=Cyc_PP_text(({const char*_tmp51A="namespace ";
_tag_dyneither(_tmp51A,sizeof(char),11);}));Cyc_PP_cat(_tag_dyneither(_tmp519,
sizeof(struct Cyc_PP_Doc*),8));});if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL26F;_LL286: {struct Cyc_Absyn_Using_d_struct*_tmp4D2=(struct Cyc_Absyn_Using_d_struct*)
_tmp4AA;if(_tmp4D2->tag != 11)goto _LL288;else{_tmp4D3=_tmp4D2->f1;_tmp4D4=_tmp4D2->f2;}}
_LL287: if(Cyc_Absynpp_print_using_stmts)s=({struct Cyc_PP_Doc*_tmp51C[8];_tmp51C[
7]=Cyc_Absynpp_rb();_tmp51C[6]=Cyc_PP_line_doc();_tmp51C[5]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({const char*_tmp51E="";_tag_dyneither(
_tmp51E,sizeof(char),1);}),_tmp4D4);_tmp51C[4]=Cyc_PP_line_doc();_tmp51C[3]=Cyc_Absynpp_lb();
_tmp51C[2]=Cyc_PP_blank_doc();_tmp51C[1]=Cyc_Absynpp_qvar2doc(_tmp4D3);_tmp51C[0]=
Cyc_PP_text(({const char*_tmp51D="using ";_tag_dyneither(_tmp51D,sizeof(char),7);}));
Cyc_PP_cat(_tag_dyneither(_tmp51C,sizeof(struct Cyc_PP_Doc*),8));});else{s=({
struct Cyc_PP_Doc*_tmp51F[11];_tmp51F[10]=Cyc_PP_text(({const char*_tmp524=" */";
_tag_dyneither(_tmp524,sizeof(char),4);}));_tmp51F[9]=Cyc_Absynpp_rb();_tmp51F[8]=
Cyc_PP_text(({const char*_tmp523="/* ";_tag_dyneither(_tmp523,sizeof(char),4);}));
_tmp51F[7]=Cyc_PP_line_doc();_tmp51F[6]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,({const char*_tmp522="";_tag_dyneither(_tmp522,sizeof(char),
1);}),_tmp4D4);_tmp51F[5]=Cyc_PP_line_doc();_tmp51F[4]=Cyc_PP_text(({const char*
_tmp521=" */";_tag_dyneither(_tmp521,sizeof(char),4);}));_tmp51F[3]=Cyc_Absynpp_lb();
_tmp51F[2]=Cyc_PP_blank_doc();_tmp51F[1]=Cyc_Absynpp_qvar2doc(_tmp4D3);_tmp51F[0]=
Cyc_PP_text(({const char*_tmp520="/* using ";_tag_dyneither(_tmp520,sizeof(char),
10);}));Cyc_PP_cat(_tag_dyneither(_tmp51F,sizeof(struct Cyc_PP_Doc*),11));});}
goto _LL26F;_LL288: {struct Cyc_Absyn_ExternC_d_struct*_tmp4D5=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp4AA;if(_tmp4D5->tag != 12)goto _LL28A;else{_tmp4D6=_tmp4D5->f1;}}_LL289: if(Cyc_Absynpp_print_externC_stmts)
s=({struct Cyc_PP_Doc*_tmp525[6];_tmp525[5]=Cyc_Absynpp_rb();_tmp525[4]=Cyc_PP_line_doc();
_tmp525[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp527="";_tag_dyneither(_tmp527,sizeof(char),1);}),_tmp4D6);_tmp525[
2]=Cyc_PP_line_doc();_tmp525[1]=Cyc_Absynpp_lb();_tmp525[0]=Cyc_PP_text(({const
char*_tmp526="extern \"C\" ";_tag_dyneither(_tmp526,sizeof(char),12);}));Cyc_PP_cat(
_tag_dyneither(_tmp525,sizeof(struct Cyc_PP_Doc*),6));});else{s=({struct Cyc_PP_Doc*
_tmp528[9];_tmp528[8]=Cyc_PP_text(({const char*_tmp52D=" */";_tag_dyneither(
_tmp52D,sizeof(char),4);}));_tmp528[7]=Cyc_Absynpp_rb();_tmp528[6]=Cyc_PP_text(({
const char*_tmp52C="/* ";_tag_dyneither(_tmp52C,sizeof(char),4);}));_tmp528[5]=
Cyc_PP_line_doc();_tmp528[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,({const char*_tmp52B="";_tag_dyneither(_tmp52B,sizeof(char),
1);}),_tmp4D6);_tmp528[3]=Cyc_PP_line_doc();_tmp528[2]=Cyc_PP_text(({const char*
_tmp52A=" */";_tag_dyneither(_tmp52A,sizeof(char),4);}));_tmp528[1]=Cyc_Absynpp_lb();
_tmp528[0]=Cyc_PP_text(({const char*_tmp529="/* extern \"C\" ";_tag_dyneither(
_tmp529,sizeof(char),15);}));Cyc_PP_cat(_tag_dyneither(_tmp528,sizeof(struct Cyc_PP_Doc*),
9));});}goto _LL26F;_LL28A: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp4D7=(
struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp4AA;if(_tmp4D7->tag != 13)goto _LL28C;
else{_tmp4D8=_tmp4D7->f1;_tmp4D9=_tmp4D7->f2;}}_LL28B: if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;if(_tmp4D9 != 0)exs_doc=({struct Cyc_PP_Doc*_tmp52E[7];
_tmp52E[6]=Cyc_Absynpp_rb();_tmp52E[5]=Cyc_PP_line_doc();_tmp52E[4]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,({const char*_tmp530=",";_tag_dyneither(
_tmp530,sizeof(char),2);}),_tmp4D9);_tmp52E[3]=Cyc_PP_line_doc();_tmp52E[2]=Cyc_Absynpp_lb();
_tmp52E[1]=Cyc_PP_text(({const char*_tmp52F=" export ";_tag_dyneither(_tmp52F,
sizeof(char),9);}));_tmp52E[0]=Cyc_Absynpp_rb();Cyc_PP_cat(_tag_dyneither(
_tmp52E,sizeof(struct Cyc_PP_Doc*),7));});else{exs_doc=Cyc_Absynpp_rb();}s=({
struct Cyc_PP_Doc*_tmp531[6];_tmp531[5]=exs_doc;_tmp531[4]=Cyc_PP_line_doc();
_tmp531[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp533="";_tag_dyneither(_tmp533,sizeof(char),1);}),_tmp4D8);_tmp531[
2]=Cyc_PP_line_doc();_tmp531[1]=Cyc_Absynpp_lb();_tmp531[0]=Cyc_PP_text(({const
char*_tmp532="extern \"C include\" ";_tag_dyneither(_tmp532,sizeof(char),20);}));
Cyc_PP_cat(_tag_dyneither(_tmp531,sizeof(struct Cyc_PP_Doc*),6));});}else{s=({
struct Cyc_PP_Doc*_tmp534[9];_tmp534[8]=Cyc_PP_text(({const char*_tmp539=" */";
_tag_dyneither(_tmp539,sizeof(char),4);}));_tmp534[7]=Cyc_Absynpp_rb();_tmp534[6]=
Cyc_PP_text(({const char*_tmp538="/* ";_tag_dyneither(_tmp538,sizeof(char),4);}));
_tmp534[5]=Cyc_PP_line_doc();_tmp534[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,({const char*_tmp537="";_tag_dyneither(_tmp537,sizeof(char),
1);}),_tmp4D8);_tmp534[3]=Cyc_PP_line_doc();_tmp534[2]=Cyc_PP_text(({const char*
_tmp536=" */";_tag_dyneither(_tmp536,sizeof(char),4);}));_tmp534[1]=Cyc_Absynpp_lb();
_tmp534[0]=Cyc_PP_text(({const char*_tmp535="/* extern \"C include\" ";
_tag_dyneither(_tmp535,sizeof(char),23);}));Cyc_PP_cat(_tag_dyneither(_tmp534,
sizeof(struct Cyc_PP_Doc*),9));});}goto _LL26F;_LL28C: {struct Cyc_Absyn_Porton_d_struct*
_tmp4DA=(struct Cyc_Absyn_Porton_d_struct*)_tmp4AA;if(_tmp4DA->tag != 14)goto
_LL28E;}_LL28D: s=({struct Cyc_PP_Doc*_tmp53A[2];_tmp53A[1]=Cyc_Absynpp_lb();
_tmp53A[0]=Cyc_PP_text(({const char*_tmp53B="__cyclone_port_on__;";_tag_dyneither(
_tmp53B,sizeof(char),21);}));Cyc_PP_cat(_tag_dyneither(_tmp53A,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL26F;_LL28E: {struct Cyc_Absyn_Portoff_d_struct*_tmp4DB=(struct Cyc_Absyn_Portoff_d_struct*)
_tmp4AA;if(_tmp4DB->tag != 15)goto _LL26F;}_LL28F: s=({struct Cyc_PP_Doc*_tmp53C[2];
_tmp53C[1]=Cyc_Absynpp_lb();_tmp53C[0]=Cyc_PP_text(({const char*_tmp53D="__cyclone_port_off__;";
_tag_dyneither(_tmp53D,sizeof(char),22);}));Cyc_PP_cat(_tag_dyneither(_tmp53C,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL26F;_LL26F:;}return s;}struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){if(Cyc_Absynpp_print_for_cycdoc)
return Cyc_PP_nil_doc();switch(sc){case Cyc_Absyn_Static: _LL299: return Cyc_PP_text(({
const char*_tmp53E="static ";_tag_dyneither(_tmp53E,sizeof(char),8);}));case Cyc_Absyn_Public:
_LL29A: return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL29B: return Cyc_PP_text(({
const char*_tmp53F="extern ";_tag_dyneither(_tmp53F,sizeof(char),8);}));case Cyc_Absyn_ExternC:
_LL29C: return Cyc_PP_text(({const char*_tmp540="extern \"C\" ";_tag_dyneither(
_tmp540,sizeof(char),12);}));case Cyc_Absyn_Abstract: _LL29D: return Cyc_PP_text(({
const char*_tmp541="abstract ";_tag_dyneither(_tmp541,sizeof(char),10);}));case
Cyc_Absyn_Register: _LL29E: return Cyc_PP_text(({const char*_tmp542="register ";
_tag_dyneither(_tmp542,sizeof(char),10);}));}}int Cyc_Absynpp_exists_temp_tvar_in_effect(
void*t){void*_tmp543=t;struct Cyc_Absyn_Tvar*_tmp545;struct Cyc_List_List*_tmp547;
_LL2A1: {struct Cyc_Absyn_VarType_struct*_tmp544=(struct Cyc_Absyn_VarType_struct*)
_tmp543;if(_tmp544->tag != 2)goto _LL2A3;else{_tmp545=_tmp544->f1;}}_LL2A2: return
Cyc_Tcutil_is_temp_tvar(_tmp545);_LL2A3: {struct Cyc_Absyn_JoinEff_struct*_tmp546=(
struct Cyc_Absyn_JoinEff_struct*)_tmp543;if(_tmp546->tag != 24)goto _LL2A5;else{
_tmp547=_tmp546->f1;}}_LL2A4: return((int(*)(int(*pred)(void*),struct Cyc_List_List*
x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp547);_LL2A5:;
_LL2A6: return 0;_LL2A0:;}int Cyc_Absynpp_is_anon_aggrtype(void*t){void*_tmp548=t;
void**_tmp54C;void*_tmp54D;_LL2A8: {struct Cyc_Absyn_AnonAggrType_struct*_tmp549=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp548;if(_tmp549->tag != 13)goto _LL2AA;}
_LL2A9: return 1;_LL2AA: {struct Cyc_Absyn_AnonEnumType_struct*_tmp54A=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp548;if(_tmp54A->tag != 15)goto _LL2AC;}_LL2AB: return 1;_LL2AC: {struct Cyc_Absyn_TypedefType_struct*
_tmp54B=(struct Cyc_Absyn_TypedefType_struct*)_tmp548;if(_tmp54B->tag != 18)goto
_LL2AE;else{_tmp54C=_tmp54B->f4;if(_tmp54C == 0)goto _LL2AE;_tmp54D=*_tmp54C;}}
_LL2AD: return Cyc_Absynpp_is_anon_aggrtype(_tmp54D);_LL2AE:;_LL2AF: return 0;_LL2A7:;}
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,
void*atts,struct Cyc_List_List*tms){if(tms != 0  && tms->tl != 0){struct _tuple16
_tmp54F=({struct _tuple16 _tmp54E;_tmp54E.f1=(void*)tms->hd;_tmp54E.f2=(void*)((
struct Cyc_List_List*)_check_null(tms->tl))->hd;_tmp54E;});void*_tmp550;void*
_tmp552;_LL2B1: _tmp550=_tmp54F.f1;{struct Cyc_Absyn_Pointer_mod_struct*_tmp551=(
struct Cyc_Absyn_Pointer_mod_struct*)_tmp550;if(_tmp551->tag != 2)goto _LL2B3;};
_tmp552=_tmp54F.f2;{struct Cyc_Absyn_Function_mod_struct*_tmp553=(struct Cyc_Absyn_Function_mod_struct*)
_tmp552;if(_tmp553->tag != 3)goto _LL2B3;};_LL2B2: return({struct Cyc_List_List*
_tmp554=_region_malloc(r,sizeof(*_tmp554));_tmp554->hd=(void*)tms->hd;_tmp554->tl=({
struct Cyc_List_List*_tmp555=_region_malloc(r,sizeof(*_tmp555));_tmp555->hd=(void*)((
struct Cyc_List_List*)_check_null(tms->tl))->hd;_tmp555->tl=Cyc_Absynpp_bubble_attributes(
r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl);_tmp555;});_tmp554;});
_LL2B3:;_LL2B4: return({struct Cyc_List_List*_tmp556=_region_malloc(r,sizeof(*
_tmp556));_tmp556->hd=atts;_tmp556->tl=tms;_tmp556;});_LL2B0:;}else{return({
struct Cyc_List_List*_tmp557=_region_malloc(r,sizeof(*_tmp557));_tmp557->hd=atts;
_tmp557->tl=tms;_tmp557;});}}struct _tuple12 Cyc_Absynpp_to_tms(struct
_RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){void*_tmp558=t;struct Cyc_Absyn_ArrayInfo
_tmp55A;void*_tmp55B;struct Cyc_Absyn_Tqual _tmp55C;struct Cyc_Absyn_Exp*_tmp55D;
union Cyc_Absyn_Constraint*_tmp55E;struct Cyc_Position_Segment*_tmp55F;struct Cyc_Absyn_PtrInfo
_tmp561;void*_tmp562;struct Cyc_Absyn_Tqual _tmp563;struct Cyc_Absyn_PtrAtts _tmp564;
struct Cyc_Absyn_FnInfo _tmp566;struct Cyc_List_List*_tmp567;struct Cyc_Core_Opt*
_tmp568;void*_tmp569;struct Cyc_List_List*_tmp56A;int _tmp56B;struct Cyc_Absyn_VarargInfo*
_tmp56C;struct Cyc_List_List*_tmp56D;struct Cyc_List_List*_tmp56E;struct Cyc_Core_Opt*
_tmp570;struct Cyc_Core_Opt*_tmp571;int _tmp572;struct _tuple0*_tmp574;struct Cyc_List_List*
_tmp575;void**_tmp576;_LL2B6: {struct Cyc_Absyn_ArrayType_struct*_tmp559=(struct
Cyc_Absyn_ArrayType_struct*)_tmp558;if(_tmp559->tag != 9)goto _LL2B8;else{_tmp55A=
_tmp559->f1;_tmp55B=_tmp55A.elt_type;_tmp55C=_tmp55A.tq;_tmp55D=_tmp55A.num_elts;
_tmp55E=_tmp55A.zero_term;_tmp55F=_tmp55A.zt_loc;}}_LL2B7: {struct Cyc_Absyn_Tqual
_tmp578;void*_tmp579;struct Cyc_List_List*_tmp57A;struct _tuple12 _tmp577=Cyc_Absynpp_to_tms(
r,_tmp55C,_tmp55B);_tmp578=_tmp577.f1;_tmp579=_tmp577.f2;_tmp57A=_tmp577.f3;{
void*tm;if(_tmp55D == 0)tm=(void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp57B=
_region_malloc(r,sizeof(*_tmp57B));_tmp57B[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp57C;_tmp57C.tag=0;_tmp57C.f1=_tmp55E;_tmp57C.f2=_tmp55F;_tmp57C;});_tmp57B;});
else{tm=(void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp57D=_region_malloc(r,
sizeof(*_tmp57D));_tmp57D[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp57E;
_tmp57E.tag=1;_tmp57E.f1=(struct Cyc_Absyn_Exp*)_tmp55D;_tmp57E.f2=_tmp55E;
_tmp57E.f3=_tmp55F;_tmp57E;});_tmp57D;});}return({struct _tuple12 _tmp57F;_tmp57F.f1=
_tmp578;_tmp57F.f2=_tmp579;_tmp57F.f3=({struct Cyc_List_List*_tmp580=
_region_malloc(r,sizeof(*_tmp580));_tmp580->hd=tm;_tmp580->tl=_tmp57A;_tmp580;});
_tmp57F;});};}_LL2B8: {struct Cyc_Absyn_PointerType_struct*_tmp560=(struct Cyc_Absyn_PointerType_struct*)
_tmp558;if(_tmp560->tag != 5)goto _LL2BA;else{_tmp561=_tmp560->f1;_tmp562=_tmp561.elt_typ;
_tmp563=_tmp561.elt_tq;_tmp564=_tmp561.ptr_atts;}}_LL2B9: {struct Cyc_Absyn_Tqual
_tmp582;void*_tmp583;struct Cyc_List_List*_tmp584;struct _tuple12 _tmp581=Cyc_Absynpp_to_tms(
r,_tmp563,_tmp562);_tmp582=_tmp581.f1;_tmp583=_tmp581.f2;_tmp584=_tmp581.f3;
_tmp584=({struct Cyc_List_List*_tmp585=_region_malloc(r,sizeof(*_tmp585));_tmp585->hd=(
void*)({struct Cyc_Absyn_Pointer_mod_struct*_tmp586=_region_malloc(r,sizeof(*
_tmp586));_tmp586[0]=({struct Cyc_Absyn_Pointer_mod_struct _tmp587;_tmp587.tag=2;
_tmp587.f1=_tmp564;_tmp587.f2=tq;_tmp587;});_tmp586;});_tmp585->tl=_tmp584;
_tmp585;});return({struct _tuple12 _tmp588;_tmp588.f1=_tmp582;_tmp588.f2=_tmp583;
_tmp588.f3=_tmp584;_tmp588;});}_LL2BA: {struct Cyc_Absyn_FnType_struct*_tmp565=(
struct Cyc_Absyn_FnType_struct*)_tmp558;if(_tmp565->tag != 10)goto _LL2BC;else{
_tmp566=_tmp565->f1;_tmp567=_tmp566.tvars;_tmp568=_tmp566.effect;_tmp569=_tmp566.ret_typ;
_tmp56A=_tmp566.args;_tmp56B=_tmp566.c_varargs;_tmp56C=_tmp566.cyc_varargs;
_tmp56D=_tmp566.rgn_po;_tmp56E=_tmp566.attributes;}}_LL2BB: if(!Cyc_Absynpp_print_all_tvars){
if(_tmp568 == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect((void*)_tmp568->v)){
_tmp568=0;_tmp567=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,
_tmp567);}{struct Cyc_Absyn_Tqual _tmp58A;void*_tmp58B;struct Cyc_List_List*_tmp58C;
struct _tuple12 _tmp589=Cyc_Absynpp_to_tms(r,Cyc_Absyn_empty_tqual(0),_tmp569);
_tmp58A=_tmp589.f1;_tmp58B=_tmp589.f2;_tmp58C=_tmp589.f3;{struct Cyc_List_List*
tms=_tmp58C;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2C2: if(
_tmp56E != 0)tms=Cyc_Absynpp_bubble_attributes(r,(void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp58D=_region_malloc(r,sizeof(*_tmp58D));_tmp58D[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp58E;_tmp58E.tag=5;_tmp58E.f1=0;_tmp58E.f2=_tmp56E;_tmp58E;});_tmp58D;}),tms);
tms=({struct Cyc_List_List*_tmp58F=_region_malloc(r,sizeof(*_tmp58F));_tmp58F->hd=(
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp590=_region_malloc(r,sizeof(*
_tmp590));_tmp590[0]=({struct Cyc_Absyn_Function_mod_struct _tmp591;_tmp591.tag=3;
_tmp591.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp592=
_region_malloc(r,sizeof(*_tmp592));_tmp592[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp593;_tmp593.tag=1;_tmp593.f1=_tmp56A;_tmp593.f2=_tmp56B;_tmp593.f3=_tmp56C;
_tmp593.f4=_tmp568;_tmp593.f5=_tmp56D;_tmp593;});_tmp592;}));_tmp591;});_tmp590;});
_tmp58F->tl=tms;_tmp58F;});break;case Cyc_Cyclone_Vc_c: _LL2C3: tms=({struct Cyc_List_List*
_tmp594=_region_malloc(r,sizeof(*_tmp594));_tmp594->hd=(void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp595=_region_malloc(r,sizeof(*_tmp595));_tmp595[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp596;_tmp596.tag=3;_tmp596.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp597=_region_malloc(r,sizeof(*_tmp597));_tmp597[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp598;_tmp598.tag=1;_tmp598.f1=_tmp56A;_tmp598.f2=_tmp56B;_tmp598.f3=_tmp56C;
_tmp598.f4=_tmp568;_tmp598.f5=_tmp56D;_tmp598;});_tmp597;}));_tmp596;});_tmp595;});
_tmp594->tl=tms;_tmp594;});for(0;_tmp56E != 0;_tmp56E=_tmp56E->tl){void*_tmp599=(
void*)_tmp56E->hd;_LL2C6: {struct Cyc_Absyn_Stdcall_att_struct*_tmp59A=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmp599;if(_tmp59A->tag != 1)goto _LL2C8;}_LL2C7: goto _LL2C9;_LL2C8: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp59B=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp599;if(_tmp59B->tag != 2)goto
_LL2CA;}_LL2C9: goto _LL2CB;_LL2CA: {struct Cyc_Absyn_Fastcall_att_struct*_tmp59C=(
struct Cyc_Absyn_Fastcall_att_struct*)_tmp599;if(_tmp59C->tag != 3)goto _LL2CC;}
_LL2CB: tms=({struct Cyc_List_List*_tmp59D=_region_malloc(r,sizeof(*_tmp59D));
_tmp59D->hd=(void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp59E=
_region_malloc(r,sizeof(*_tmp59E));_tmp59E[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp59F;_tmp59F.tag=5;_tmp59F.f1=0;_tmp59F.f2=({struct Cyc_List_List*_tmp5A0=
_cycalloc(sizeof(*_tmp5A0));_tmp5A0->hd=(void*)_tmp56E->hd;_tmp5A0->tl=0;_tmp5A0;});
_tmp59F;});_tmp59E;});_tmp59D->tl=tms;_tmp59D;});goto AfterAtts;_LL2CC:;_LL2CD:
goto _LL2C5;_LL2C5:;}break;}AfterAtts: if(_tmp567 != 0)tms=({struct Cyc_List_List*
_tmp5A1=_region_malloc(r,sizeof(*_tmp5A1));_tmp5A1->hd=(void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp5A2=_region_malloc(r,sizeof(*_tmp5A2));_tmp5A2[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp5A3;_tmp5A3.tag=4;_tmp5A3.f1=_tmp567;_tmp5A3.f2=0;_tmp5A3.f3=1;_tmp5A3;});
_tmp5A2;});_tmp5A1->tl=tms;_tmp5A1;});return({struct _tuple12 _tmp5A4;_tmp5A4.f1=
_tmp58A;_tmp5A4.f2=_tmp58B;_tmp5A4.f3=tms;_tmp5A4;});};};_LL2BC: {struct Cyc_Absyn_Evar_struct*
_tmp56F=(struct Cyc_Absyn_Evar_struct*)_tmp558;if(_tmp56F->tag != 1)goto _LL2BE;
else{_tmp570=_tmp56F->f1;_tmp571=_tmp56F->f2;_tmp572=_tmp56F->f3;}}_LL2BD: if(
_tmp571 == 0)return({struct _tuple12 _tmp5A5;_tmp5A5.f1=tq;_tmp5A5.f2=t;_tmp5A5.f3=
0;_tmp5A5;});else{return Cyc_Absynpp_to_tms(r,tq,(void*)_tmp571->v);}_LL2BE: {
struct Cyc_Absyn_TypedefType_struct*_tmp573=(struct Cyc_Absyn_TypedefType_struct*)
_tmp558;if(_tmp573->tag != 18)goto _LL2C0;else{_tmp574=_tmp573->f1;_tmp575=_tmp573->f2;
_tmp576=_tmp573->f4;}}_LL2BF: if((_tmp576 == 0  || !Cyc_Absynpp_expand_typedefs)
 || Cyc_Absynpp_is_anon_aggrtype(*_tmp576))return({struct _tuple12 _tmp5A6;_tmp5A6.f1=
tq;_tmp5A6.f2=t;_tmp5A6.f3=0;_tmp5A6;});else{return Cyc_Absynpp_to_tms(r,tq,*
_tmp576);}_LL2C0:;_LL2C1: return({struct _tuple12 _tmp5A7;_tmp5A7.f1=tq;_tmp5A7.f2=
t;_tmp5A7.f3=0;_tmp5A7;});_LL2B5:;}static int Cyc_Absynpp_is_char_ptr(void*t){void*
_tmp5A8=t;struct Cyc_Core_Opt*_tmp5AA;struct Cyc_Core_Opt _tmp5AB;void*_tmp5AC;
struct Cyc_Absyn_PtrInfo _tmp5AE;void*_tmp5AF;_LL2CF: {struct Cyc_Absyn_Evar_struct*
_tmp5A9=(struct Cyc_Absyn_Evar_struct*)_tmp5A8;if(_tmp5A9->tag != 1)goto _LL2D1;
else{_tmp5AA=_tmp5A9->f2;if(_tmp5AA == 0)goto _LL2D1;_tmp5AB=*_tmp5AA;_tmp5AC=(
void*)_tmp5AB.v;}}_LL2D0: return Cyc_Absynpp_is_char_ptr(_tmp5AC);_LL2D1: {struct
Cyc_Absyn_PointerType_struct*_tmp5AD=(struct Cyc_Absyn_PointerType_struct*)
_tmp5A8;if(_tmp5AD->tag != 5)goto _LL2D3;else{_tmp5AE=_tmp5AD->f1;_tmp5AF=_tmp5AE.elt_typ;}}
_LL2D2: L: {void*_tmp5B0=_tmp5AF;struct Cyc_Core_Opt*_tmp5B2;struct Cyc_Core_Opt
_tmp5B3;void*_tmp5B4;void**_tmp5B6;void*_tmp5B7;enum Cyc_Absyn_Size_of _tmp5B9;
_LL2D6: {struct Cyc_Absyn_Evar_struct*_tmp5B1=(struct Cyc_Absyn_Evar_struct*)
_tmp5B0;if(_tmp5B1->tag != 1)goto _LL2D8;else{_tmp5B2=_tmp5B1->f2;if(_tmp5B2 == 0)
goto _LL2D8;_tmp5B3=*_tmp5B2;_tmp5B4=(void*)_tmp5B3.v;}}_LL2D7: _tmp5AF=_tmp5B4;
goto L;_LL2D8: {struct Cyc_Absyn_TypedefType_struct*_tmp5B5=(struct Cyc_Absyn_TypedefType_struct*)
_tmp5B0;if(_tmp5B5->tag != 18)goto _LL2DA;else{_tmp5B6=_tmp5B5->f4;if(_tmp5B6 == 0)
goto _LL2DA;_tmp5B7=*_tmp5B6;}}_LL2D9: _tmp5AF=_tmp5B7;goto L;_LL2DA: {struct Cyc_Absyn_IntType_struct*
_tmp5B8=(struct Cyc_Absyn_IntType_struct*)_tmp5B0;if(_tmp5B8->tag != 6)goto _LL2DC;
else{_tmp5B9=_tmp5B8->f2;if(_tmp5B9 != Cyc_Absyn_Char_sz)goto _LL2DC;}}_LL2DB:
return 1;_LL2DC:;_LL2DD: return 0;_LL2D5:;}_LL2D3:;_LL2D4: return 0;_LL2CE:;}struct
Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*
dopt){struct _RegionHandle _tmp5BA=_new_region("temp");struct _RegionHandle*temp=&
_tmp5BA;_push_region(temp);{struct Cyc_Absyn_Tqual _tmp5BC;void*_tmp5BD;struct Cyc_List_List*
_tmp5BE;struct _tuple12 _tmp5BB=Cyc_Absynpp_to_tms(temp,tq,typ);_tmp5BC=_tmp5BB.f1;
_tmp5BD=_tmp5BB.f2;_tmp5BE=_tmp5BB.f3;_tmp5BE=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5BE);if(_tmp5BE == 0  && dopt == 0){struct
Cyc_PP_Doc*_tmp5C0=({struct Cyc_PP_Doc*_tmp5BF[2];_tmp5BF[1]=Cyc_Absynpp_ntyp2doc(
_tmp5BD);_tmp5BF[0]=Cyc_Absynpp_tqual2doc(_tmp5BC);Cyc_PP_cat(_tag_dyneither(
_tmp5BF,sizeof(struct Cyc_PP_Doc*),2));});_npop_handler(0);return _tmp5C0;}else{
struct Cyc_PP_Doc*_tmp5C3=({struct Cyc_PP_Doc*_tmp5C1[4];_tmp5C1[3]=Cyc_Absynpp_dtms2doc(
Cyc_Absynpp_is_char_ptr(typ),dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,
_tmp5BE);_tmp5C1[2]=Cyc_PP_text(({const char*_tmp5C2=" ";_tag_dyneither(_tmp5C2,
sizeof(char),2);}));_tmp5C1[1]=Cyc_Absynpp_ntyp2doc(_tmp5BD);_tmp5C1[0]=Cyc_Absynpp_tqual2doc(
_tmp5BC);Cyc_PP_cat(_tag_dyneither(_tmp5C1,sizeof(struct Cyc_PP_Doc*),4));});
_npop_handler(0);return _tmp5C3;}};_pop_region(temp);}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(
struct Cyc_Absyn_Aggrfield*f){switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c:
_LL2DE: if(f->width != 0)return({struct Cyc_PP_Doc*_tmp5C4[5];_tmp5C4[4]=Cyc_PP_text(({
const char*_tmp5C7=";";_tag_dyneither(_tmp5C7,sizeof(char),2);}));_tmp5C4[3]=Cyc_Absynpp_atts2doc(
f->attributes);_tmp5C4[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(
f->width));_tmp5C4[1]=Cyc_PP_text(({const char*_tmp5C6=":";_tag_dyneither(_tmp5C6,
sizeof(char),2);}));_tmp5C4[0]=Cyc_Absynpp_tqtd2doc(f->tq,f->type,({struct Cyc_Core_Opt*
_tmp5C5=_cycalloc(sizeof(*_tmp5C5));_tmp5C5->v=Cyc_PP_textptr(f->name);_tmp5C5;}));
Cyc_PP_cat(_tag_dyneither(_tmp5C4,sizeof(struct Cyc_PP_Doc*),5));});else{return({
struct Cyc_PP_Doc*_tmp5C8[3];_tmp5C8[2]=Cyc_PP_text(({const char*_tmp5CA=";";
_tag_dyneither(_tmp5CA,sizeof(char),2);}));_tmp5C8[1]=Cyc_Absynpp_atts2doc(f->attributes);
_tmp5C8[0]=Cyc_Absynpp_tqtd2doc(f->tq,f->type,({struct Cyc_Core_Opt*_tmp5C9=
_cycalloc(sizeof(*_tmp5C9));_tmp5C9->v=Cyc_PP_textptr(f->name);_tmp5C9;}));Cyc_PP_cat(
_tag_dyneither(_tmp5C8,sizeof(struct Cyc_PP_Doc*),3));});}case Cyc_Cyclone_Vc_c:
_LL2DF: if(f->width != 0)return({struct Cyc_PP_Doc*_tmp5CB[5];_tmp5CB[4]=Cyc_PP_text(({
const char*_tmp5CE=";";_tag_dyneither(_tmp5CE,sizeof(char),2);}));_tmp5CB[3]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp5CB[2]=Cyc_PP_text(({const char*
_tmp5CD=":";_tag_dyneither(_tmp5CD,sizeof(char),2);}));_tmp5CB[1]=Cyc_Absynpp_tqtd2doc(
f->tq,f->type,({struct Cyc_Core_Opt*_tmp5CC=_cycalloc(sizeof(*_tmp5CC));_tmp5CC->v=
Cyc_PP_textptr(f->name);_tmp5CC;}));_tmp5CB[0]=Cyc_Absynpp_atts2doc(f->attributes);
Cyc_PP_cat(_tag_dyneither(_tmp5CB,sizeof(struct Cyc_PP_Doc*),5));});else{return({
struct Cyc_PP_Doc*_tmp5CF[3];_tmp5CF[2]=Cyc_PP_text(({const char*_tmp5D1=";";
_tag_dyneither(_tmp5D1,sizeof(char),2);}));_tmp5CF[1]=Cyc_Absynpp_tqtd2doc(f->tq,
f->type,({struct Cyc_Core_Opt*_tmp5D0=_cycalloc(sizeof(*_tmp5D0));_tmp5D0->v=Cyc_PP_textptr(
f->name);_tmp5D0;}));_tmp5CF[0]=Cyc_Absynpp_atts2doc(f->attributes);Cyc_PP_cat(
_tag_dyneither(_tmp5CF,sizeof(struct Cyc_PP_Doc*),3));});}}}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(
struct Cyc_List_List*fields){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_aggrfield2doc,({const char*_tmp5D2="";_tag_dyneither(_tmp5D2,sizeof(
char),1);}),fields);}struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*
f){return({struct Cyc_PP_Doc*_tmp5D3[3];_tmp5D3[2]=f->typs == 0?Cyc_PP_nil_doc():
Cyc_Absynpp_args2doc(f->typs);_tmp5D3[1]=Cyc_Absynpp_typedef_name2doc(f->name);
_tmp5D3[0]=Cyc_Absynpp_scope2doc(f->sc);Cyc_PP_cat(_tag_dyneither(_tmp5D3,
sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(
struct Cyc_List_List*fields){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Datatypefield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))
Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,({const char*_tmp5D4=",";
_tag_dyneither(_tmp5D4,sizeof(char),2);}),fields);}void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){for(0;tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(
Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);({void*_tmp5D5=0;Cyc_fprintf(
f,({const char*_tmp5D6="\n";_tag_dyneither(_tmp5D6,sizeof(char),2);}),
_tag_dyneither(_tmp5D5,sizeof(void*),0));});}}struct _dyneither_ptr Cyc_Absynpp_decllist2string(
struct Cyc_List_List*tdl){return Cyc_PP_string_of_doc(Cyc_PP_seql(({const char*
_tmp5D7="";_tag_dyneither(_tmp5D7,sizeof(char),1);}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_decl2doc,tdl)),72);}struct _dyneither_ptr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(Cyc_Absynpp_exp2doc(e),72);}
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(
Cyc_Absynpp_stmt2doc(s,0),72);}struct _dyneither_ptr Cyc_Absynpp_typ2string(void*t){
return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}struct _dyneither_ptr Cyc_Absynpp_typ2cstring(
void*t){int old_qvar_to_Cids=Cyc_Absynpp_qvar_to_Cids;int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;
Cyc_Absynpp_qvar_to_Cids=1;Cyc_Absynpp_add_cyc_prefix=0;{struct _dyneither_ptr s=
Cyc_Absynpp_typ2string(t);Cyc_Absynpp_qvar_to_Cids=old_qvar_to_Cids;Cyc_Absynpp_add_cyc_prefix=
old_add_cyc_prefix;return s;};}struct _dyneither_ptr Cyc_Absynpp_prim2string(enum 
Cyc_Absyn_Primop p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(
Cyc_Absynpp_pat2doc(p),72);}struct _dyneither_ptr Cyc_Absynpp_scope2string(enum 
Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
