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
v){struct Cyc_List_List*_tmp5DF;Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,((
_tmp5DF=_cycalloc(sizeof(*_tmp5DF)),((_tmp5DF->hd=v,((_tmp5DF->tl=0,_tmp5DF)))))));}
static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l);static void Cyc_Absynpp_suppr_last(
struct Cyc_List_List**l){if(((struct Cyc_List_List*)_check_null(*l))->tl == 0)*l=0;
else{Cyc_Absynpp_suppr_last(&((struct Cyc_List_List*)_check_null(*l))->tl);}}
static void Cyc_Absynpp_curr_namespace_drop();static void Cyc_Absynpp_curr_namespace_drop(){((
void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
struct _dyneither_ptr Cyc_Absynpp_char_escape(char c);struct _dyneither_ptr Cyc_Absynpp_char_escape(
char c){switch(c){case '\a': _LL0: {const char*_tmp5E0;return(_tmp5E0="\\a",
_tag_dyneither(_tmp5E0,sizeof(char),3));}case '\b': _LL1: {const char*_tmp5E1;
return(_tmp5E1="\\b",_tag_dyneither(_tmp5E1,sizeof(char),3));}case '\f': _LL2: {
const char*_tmp5E2;return(_tmp5E2="\\f",_tag_dyneither(_tmp5E2,sizeof(char),3));}
case '\n': _LL3: {const char*_tmp5E3;return(_tmp5E3="\\n",_tag_dyneither(_tmp5E3,
sizeof(char),3));}case '\r': _LL4: {const char*_tmp5E4;return(_tmp5E4="\\r",
_tag_dyneither(_tmp5E4,sizeof(char),3));}case '\t': _LL5: {const char*_tmp5E5;
return(_tmp5E5="\\t",_tag_dyneither(_tmp5E5,sizeof(char),3));}case '\v': _LL6: {
const char*_tmp5E6;return(_tmp5E6="\\v",_tag_dyneither(_tmp5E6,sizeof(char),3));}
case '\\': _LL7: {const char*_tmp5E7;return(_tmp5E7="\\\\",_tag_dyneither(_tmp5E7,
sizeof(char),3));}case '"': _LL8: {const char*_tmp5E8;return(_tmp5E8="\"",
_tag_dyneither(_tmp5E8,sizeof(char),2));}case '\'': _LL9: {const char*_tmp5E9;
return(_tmp5E9="\\'",_tag_dyneither(_tmp5E9,sizeof(char),3));}default: _LLA: if(c
>= ' '  && c <= '~'){struct _dyneither_ptr _tmpC=Cyc_Core_new_string(2);{char _tmp5EC;
char _tmp5EB;struct _dyneither_ptr _tmp5EA;(_tmp5EA=_dyneither_ptr_plus(_tmpC,
sizeof(char),0),((_tmp5EB=*((char*)_check_dyneither_subscript(_tmp5EA,sizeof(
char),0)),((_tmp5EC=c,((_get_dyneither_size(_tmp5EA,sizeof(char))== 1  && (
_tmp5EB == '\000'  && _tmp5EC != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5EA.curr)=
_tmp5EC)))))));}return(struct _dyneither_ptr)_tmpC;}else{struct _dyneither_ptr
_tmp10=Cyc_Core_new_string(5);int j=0;{char _tmp5EF;char _tmp5EE;struct
_dyneither_ptr _tmp5ED;(_tmp5ED=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((
_tmp5EE=*((char*)_check_dyneither_subscript(_tmp5ED,sizeof(char),0)),((_tmp5EF='\\',((
_get_dyneither_size(_tmp5ED,sizeof(char))== 1  && (_tmp5EE == '\000'  && _tmp5EF != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp5ED.curr)=_tmp5EF)))))));}{char _tmp5F2;char
_tmp5F1;struct _dyneither_ptr _tmp5F0;(_tmp5F0=_dyneither_ptr_plus(_tmp10,sizeof(
char),j ++),((_tmp5F1=*((char*)_check_dyneither_subscript(_tmp5F0,sizeof(char),0)),((
_tmp5F2=(char)('0' + ((unsigned char)c >> 6 & 3)),((_get_dyneither_size(_tmp5F0,
sizeof(char))== 1  && (_tmp5F1 == '\000'  && _tmp5F2 != '\000')?_throw_arraybounds():
1,*((char*)_tmp5F0.curr)=_tmp5F2)))))));}{char _tmp5F5;char _tmp5F4;struct
_dyneither_ptr _tmp5F3;(_tmp5F3=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((
_tmp5F4=*((char*)_check_dyneither_subscript(_tmp5F3,sizeof(char),0)),((_tmp5F5=(
char)('0' + (c >> 3 & 7)),((_get_dyneither_size(_tmp5F3,sizeof(char))== 1  && (
_tmp5F4 == '\000'  && _tmp5F5 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5F3.curr)=
_tmp5F5)))))));}{char _tmp5F8;char _tmp5F7;struct _dyneither_ptr _tmp5F6;(_tmp5F6=
_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp5F7=*((char*)
_check_dyneither_subscript(_tmp5F6,sizeof(char),0)),((_tmp5F8=(char)('0' + (c & 7)),((
_get_dyneither_size(_tmp5F6,sizeof(char))== 1  && (_tmp5F7 == '\000'  && _tmp5F8 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp5F6.curr)=_tmp5F8)))))));}return(struct
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
_LL22: if(_tmp1E != '\a')goto _LL24;_LL23:{char _tmp5FB;char _tmp5FA;struct
_dyneither_ptr _tmp5F9;(_tmp5F9=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5FA=*((
char*)_check_dyneither_subscript(_tmp5F9,sizeof(char),0)),((_tmp5FB='\\',((
_get_dyneither_size(_tmp5F9,sizeof(char))== 1  && (_tmp5FA == '\000'  && _tmp5FB != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp5F9.curr)=_tmp5FB)))))));}{char _tmp5FE;char
_tmp5FD;struct _dyneither_ptr _tmp5FC;(_tmp5FC=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp5FD=*((char*)_check_dyneither_subscript(_tmp5FC,sizeof(char),0)),((
_tmp5FE='a',((_get_dyneither_size(_tmp5FC,sizeof(char))== 1  && (_tmp5FD == '\000'
 && _tmp5FE != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5FC.curr)=_tmp5FE)))))));}
goto _LL21;_LL24: if(_tmp1E != '\b')goto _LL26;_LL25:{char _tmp601;char _tmp600;struct
_dyneither_ptr _tmp5FF;(_tmp5FF=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp600=*((
char*)_check_dyneither_subscript(_tmp5FF,sizeof(char),0)),((_tmp601='\\',((
_get_dyneither_size(_tmp5FF,sizeof(char))== 1  && (_tmp600 == '\000'  && _tmp601 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp5FF.curr)=_tmp601)))))));}{char _tmp604;char
_tmp603;struct _dyneither_ptr _tmp602;(_tmp602=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp603=*((char*)_check_dyneither_subscript(_tmp602,sizeof(char),0)),((
_tmp604='b',((_get_dyneither_size(_tmp602,sizeof(char))== 1  && (_tmp603 == '\000'
 && _tmp604 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp602.curr)=_tmp604)))))));}
goto _LL21;_LL26: if(_tmp1E != '\f')goto _LL28;_LL27:{char _tmp607;char _tmp606;struct
_dyneither_ptr _tmp605;(_tmp605=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp606=*((
char*)_check_dyneither_subscript(_tmp605,sizeof(char),0)),((_tmp607='\\',((
_get_dyneither_size(_tmp605,sizeof(char))== 1  && (_tmp606 == '\000'  && _tmp607 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp605.curr)=_tmp607)))))));}{char _tmp60A;char
_tmp609;struct _dyneither_ptr _tmp608;(_tmp608=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp609=*((char*)_check_dyneither_subscript(_tmp608,sizeof(char),0)),((
_tmp60A='f',((_get_dyneither_size(_tmp608,sizeof(char))== 1  && (_tmp609 == '\000'
 && _tmp60A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp608.curr)=_tmp60A)))))));}
goto _LL21;_LL28: if(_tmp1E != '\n')goto _LL2A;_LL29:{char _tmp60D;char _tmp60C;struct
_dyneither_ptr _tmp60B;(_tmp60B=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp60C=*((
char*)_check_dyneither_subscript(_tmp60B,sizeof(char),0)),((_tmp60D='\\',((
_get_dyneither_size(_tmp60B,sizeof(char))== 1  && (_tmp60C == '\000'  && _tmp60D != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp60B.curr)=_tmp60D)))))));}{char _tmp610;char
_tmp60F;struct _dyneither_ptr _tmp60E;(_tmp60E=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp60F=*((char*)_check_dyneither_subscript(_tmp60E,sizeof(char),0)),((
_tmp610='n',((_get_dyneither_size(_tmp60E,sizeof(char))== 1  && (_tmp60F == '\000'
 && _tmp610 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp60E.curr)=_tmp610)))))));}
goto _LL21;_LL2A: if(_tmp1E != '\r')goto _LL2C;_LL2B:{char _tmp613;char _tmp612;struct
_dyneither_ptr _tmp611;(_tmp611=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp612=*((
char*)_check_dyneither_subscript(_tmp611,sizeof(char),0)),((_tmp613='\\',((
_get_dyneither_size(_tmp611,sizeof(char))== 1  && (_tmp612 == '\000'  && _tmp613 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp611.curr)=_tmp613)))))));}{char _tmp616;char
_tmp615;struct _dyneither_ptr _tmp614;(_tmp614=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp615=*((char*)_check_dyneither_subscript(_tmp614,sizeof(char),0)),((
_tmp616='r',((_get_dyneither_size(_tmp614,sizeof(char))== 1  && (_tmp615 == '\000'
 && _tmp616 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp614.curr)=_tmp616)))))));}
goto _LL21;_LL2C: if(_tmp1E != '\t')goto _LL2E;_LL2D:{char _tmp619;char _tmp618;struct
_dyneither_ptr _tmp617;(_tmp617=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp618=*((
char*)_check_dyneither_subscript(_tmp617,sizeof(char),0)),((_tmp619='\\',((
_get_dyneither_size(_tmp617,sizeof(char))== 1  && (_tmp618 == '\000'  && _tmp619 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp617.curr)=_tmp619)))))));}{char _tmp61C;char
_tmp61B;struct _dyneither_ptr _tmp61A;(_tmp61A=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp61B=*((char*)_check_dyneither_subscript(_tmp61A,sizeof(char),0)),((
_tmp61C='t',((_get_dyneither_size(_tmp61A,sizeof(char))== 1  && (_tmp61B == '\000'
 && _tmp61C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp61A.curr)=_tmp61C)))))));}
goto _LL21;_LL2E: if(_tmp1E != '\v')goto _LL30;_LL2F:{char _tmp61F;char _tmp61E;struct
_dyneither_ptr _tmp61D;(_tmp61D=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp61E=*((
char*)_check_dyneither_subscript(_tmp61D,sizeof(char),0)),((_tmp61F='\\',((
_get_dyneither_size(_tmp61D,sizeof(char))== 1  && (_tmp61E == '\000'  && _tmp61F != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp61D.curr)=_tmp61F)))))));}{char _tmp622;char
_tmp621;struct _dyneither_ptr _tmp620;(_tmp620=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp621=*((char*)_check_dyneither_subscript(_tmp620,sizeof(char),0)),((
_tmp622='v',((_get_dyneither_size(_tmp620,sizeof(char))== 1  && (_tmp621 == '\000'
 && _tmp622 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp620.curr)=_tmp622)))))));}
goto _LL21;_LL30: if(_tmp1E != '\\')goto _LL32;_LL31:{char _tmp625;char _tmp624;struct
_dyneither_ptr _tmp623;(_tmp623=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp624=*((
char*)_check_dyneither_subscript(_tmp623,sizeof(char),0)),((_tmp625='\\',((
_get_dyneither_size(_tmp623,sizeof(char))== 1  && (_tmp624 == '\000'  && _tmp625 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp623.curr)=_tmp625)))))));}{char _tmp628;char
_tmp627;struct _dyneither_ptr _tmp626;(_tmp626=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp627=*((char*)_check_dyneither_subscript(_tmp626,sizeof(char),0)),((
_tmp628='\\',((_get_dyneither_size(_tmp626,sizeof(char))== 1  && (_tmp627 == '\000'
 && _tmp628 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp626.curr)=_tmp628)))))));}
goto _LL21;_LL32: if(_tmp1E != '"')goto _LL34;_LL33:{char _tmp62B;char _tmp62A;struct
_dyneither_ptr _tmp629;(_tmp629=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp62A=*((
char*)_check_dyneither_subscript(_tmp629,sizeof(char),0)),((_tmp62B='\\',((
_get_dyneither_size(_tmp629,sizeof(char))== 1  && (_tmp62A == '\000'  && _tmp62B != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp629.curr)=_tmp62B)))))));}{char _tmp62E;char
_tmp62D;struct _dyneither_ptr _tmp62C;(_tmp62C=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp62D=*((char*)_check_dyneither_subscript(_tmp62C,sizeof(char),0)),((
_tmp62E='"',((_get_dyneither_size(_tmp62C,sizeof(char))== 1  && (_tmp62D == '\000'
 && _tmp62E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp62C.curr)=_tmp62E)))))));}
goto _LL21;_LL34:;_LL35: if(_tmp1E >= ' '  && _tmp1E <= '~'){char _tmp631;char _tmp630;
struct _dyneither_ptr _tmp62F;(_tmp62F=_dyneither_ptr_plus(t,sizeof(char),j ++),((
_tmp630=*((char*)_check_dyneither_subscript(_tmp62F,sizeof(char),0)),((_tmp631=
_tmp1E,((_get_dyneither_size(_tmp62F,sizeof(char))== 1  && (_tmp630 == '\000'  && 
_tmp631 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp62F.curr)=_tmp631)))))));}
else{{char _tmp634;char _tmp633;struct _dyneither_ptr _tmp632;(_tmp632=
_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp633=*((char*)
_check_dyneither_subscript(_tmp632,sizeof(char),0)),((_tmp634='\\',((
_get_dyneither_size(_tmp632,sizeof(char))== 1  && (_tmp633 == '\000'  && _tmp634 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp632.curr)=_tmp634)))))));}{char _tmp637;char
_tmp636;struct _dyneither_ptr _tmp635;(_tmp635=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp636=*((char*)_check_dyneither_subscript(_tmp635,sizeof(char),0)),((
_tmp637=(char)('0' + (_tmp1E >> 6 & 7)),((_get_dyneither_size(_tmp635,sizeof(char))
== 1  && (_tmp636 == '\000'  && _tmp637 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp635.curr)=_tmp637)))))));}{char _tmp63A;char _tmp639;struct _dyneither_ptr
_tmp638;(_tmp638=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp639=*((char*)
_check_dyneither_subscript(_tmp638,sizeof(char),0)),((_tmp63A=(char)('0' + (
_tmp1E >> 3 & 7)),((_get_dyneither_size(_tmp638,sizeof(char))== 1  && (_tmp639 == '\000'
 && _tmp63A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp638.curr)=_tmp63A)))))));}{
char _tmp63D;char _tmp63C;struct _dyneither_ptr _tmp63B;(_tmp63B=_dyneither_ptr_plus(
t,sizeof(char),j ++),((_tmp63C=*((char*)_check_dyneither_subscript(_tmp63B,
sizeof(char),0)),((_tmp63D=(char)('0' + (_tmp1E & 7)),((_get_dyneither_size(
_tmp63B,sizeof(char))== 1  && (_tmp63C == '\000'  && _tmp63D != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp63B.curr)=_tmp63D)))))));};}goto _LL21;_LL21:;}}
return(struct _dyneither_ptr)t;};};};}static char _tmp64[9]="restrict";static struct
_dyneither_ptr Cyc_Absynpp_restrict_string={_tmp64,_tmp64,_tmp64 + 9};static char
_tmp65[9]="volatile";static struct _dyneither_ptr Cyc_Absynpp_volatile_string={
_tmp65,_tmp65,_tmp65 + 9};static char _tmp66[6]="const";static struct _dyneither_ptr
Cyc_Absynpp_const_str={_tmp66,_tmp66,_tmp66 + 6};static struct _dyneither_ptr*Cyc_Absynpp_restrict_sp=&
Cyc_Absynpp_restrict_string;static struct _dyneither_ptr*Cyc_Absynpp_volatile_sp=&
Cyc_Absynpp_volatile_string;static struct _dyneither_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq);struct Cyc_PP_Doc*
Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){struct Cyc_List_List*l=0;if(tq.q_restrict){
struct Cyc_List_List*_tmp63E;l=((_tmp63E=_cycalloc(sizeof(*_tmp63E)),((_tmp63E->hd=
Cyc_Absynpp_restrict_sp,((_tmp63E->tl=l,_tmp63E))))));}if(tq.q_volatile){struct
Cyc_List_List*_tmp63F;l=((_tmp63F=_cycalloc(sizeof(*_tmp63F)),((_tmp63F->hd=Cyc_Absynpp_volatile_sp,((
_tmp63F->tl=l,_tmp63F))))));}if(tq.print_const){struct Cyc_List_List*_tmp640;l=((
_tmp640=_cycalloc(sizeof(*_tmp640)),((_tmp640->hd=Cyc_Absynpp_const_sp,((_tmp640->tl=
l,_tmp640))))));}{const char*_tmp643;const char*_tmp642;const char*_tmp641;return
Cyc_PP_egroup(((_tmp641="",_tag_dyneither(_tmp641,sizeof(char),1))),((_tmp642=" ",
_tag_dyneither(_tmp642,sizeof(char),2))),((_tmp643=" ",_tag_dyneither(_tmp643,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));};}
struct _dyneither_ptr Cyc_Absynpp_kind2string(enum Cyc_Absyn_Kind k);struct
_dyneither_ptr Cyc_Absynpp_kind2string(enum Cyc_Absyn_Kind k){switch(k){case Cyc_Absyn_AnyKind:
_LL36: {const char*_tmp644;return(_tmp644="A",_tag_dyneither(_tmp644,sizeof(char),
2));}case Cyc_Absyn_MemKind: _LL37: {const char*_tmp645;return(_tmp645="M",
_tag_dyneither(_tmp645,sizeof(char),2));}case Cyc_Absyn_BoxKind: _LL38: {const char*
_tmp646;return(_tmp646="B",_tag_dyneither(_tmp646,sizeof(char),2));}case Cyc_Absyn_RgnKind:
_LL39: {const char*_tmp647;return(_tmp647="R",_tag_dyneither(_tmp647,sizeof(char),
2));}case Cyc_Absyn_UniqueRgnKind: _LL3A: {const char*_tmp648;return(_tmp648="UR",
_tag_dyneither(_tmp648,sizeof(char),3));}case Cyc_Absyn_TopRgnKind: _LL3B: {const
char*_tmp649;return(_tmp649="TR",_tag_dyneither(_tmp649,sizeof(char),3));}case
Cyc_Absyn_EffKind: _LL3C: {const char*_tmp64A;return(_tmp64A="E",_tag_dyneither(
_tmp64A,sizeof(char),2));}case Cyc_Absyn_IntKind: _LL3D: {const char*_tmp64B;return(
_tmp64B="I",_tag_dyneither(_tmp64B,sizeof(char),2));}}}struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(
enum Cyc_Absyn_Kind k);struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(enum Cyc_Absyn_Kind k){
return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}struct _dyneither_ptr Cyc_Absynpp_kindbound2string(
void*c);struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){void*_tmp75=Cyc_Absyn_compress_kb(
c);enum Cyc_Absyn_Kind _tmp77;enum Cyc_Absyn_Kind _tmp7A;_LL40: {struct Cyc_Absyn_Eq_kb_struct*
_tmp76=(struct Cyc_Absyn_Eq_kb_struct*)_tmp75;if(_tmp76->tag != 0)goto _LL42;else{
_tmp77=_tmp76->f1;}}_LL41: return Cyc_Absynpp_kind2string(_tmp77);_LL42: {struct
Cyc_Absyn_Unknown_kb_struct*_tmp78=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp75;
if(_tmp78->tag != 1)goto _LL44;}_LL43: if(Cyc_PP_tex_output){const char*_tmp64C;
return(_tmp64C="{?}",_tag_dyneither(_tmp64C,sizeof(char),4));}else{const char*
_tmp64D;return(_tmp64D="?",_tag_dyneither(_tmp64D,sizeof(char),2));}_LL44: {
struct Cyc_Absyn_Less_kb_struct*_tmp79=(struct Cyc_Absyn_Less_kb_struct*)_tmp75;
if(_tmp79->tag != 2)goto _LL3F;else{_tmp7A=_tmp79->f2;}}_LL45: return Cyc_Absynpp_kind2string(
_tmp7A);_LL3F:;}struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c);struct Cyc_PP_Doc*
Cyc_Absynpp_kindbound2doc(void*c){void*_tmp7D=Cyc_Absyn_compress_kb(c);enum Cyc_Absyn_Kind
_tmp7F;enum Cyc_Absyn_Kind _tmp82;_LL47: {struct Cyc_Absyn_Eq_kb_struct*_tmp7E=(
struct Cyc_Absyn_Eq_kb_struct*)_tmp7D;if(_tmp7E->tag != 0)goto _LL49;else{_tmp7F=
_tmp7E->f1;}}_LL48: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp7F));_LL49: {
struct Cyc_Absyn_Unknown_kb_struct*_tmp80=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp7D;if(_tmp80->tag != 1)goto _LL4B;}_LL4A: if(Cyc_PP_tex_output){const char*
_tmp64E;return Cyc_PP_text_width(((_tmp64E="{?}",_tag_dyneither(_tmp64E,sizeof(
char),4))),1);}else{const char*_tmp64F;return Cyc_PP_text(((_tmp64F="?",
_tag_dyneither(_tmp64F,sizeof(char),2))));}_LL4B: {struct Cyc_Absyn_Less_kb_struct*
_tmp81=(struct Cyc_Absyn_Less_kb_struct*)_tmp7D;if(_tmp81->tag != 2)goto _LL46;
else{_tmp82=_tmp81->f2;}}_LL4C: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp82));
_LL46:;}struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts);struct Cyc_PP_Doc*
Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){const char*_tmp652;const char*_tmp651;
const char*_tmp650;return Cyc_PP_egroup(((_tmp650="<",_tag_dyneither(_tmp650,
sizeof(char),2))),((_tmp651=">",_tag_dyneither(_tmp651,sizeof(char),2))),((
_tmp652=",",_tag_dyneither(_tmp652,sizeof(char),2))),((struct Cyc_List_List*(*)(
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
_tmp655;struct Cyc_PP_Doc*_tmp654[3];return(_tmp654[2]=Cyc_Absynpp_kind2doc(
_tmp8F),((_tmp654[1]=Cyc_PP_text(((_tmp655="::",_tag_dyneither(_tmp655,sizeof(
char),3)))),((_tmp654[0]=Cyc_PP_textptr(tv->name),Cyc_PP_cat(_tag_dyneither(
_tmp654,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL4D:;}struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(
struct Cyc_List_List*tvs);struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*
tvs){const char*_tmp658;const char*_tmp657;const char*_tmp656;return Cyc_PP_egroup(((
_tmp656="<",_tag_dyneither(_tmp656,sizeof(char),2))),((_tmp657=">",
_tag_dyneither(_tmp657,sizeof(char),2))),((_tmp658=",",_tag_dyneither(_tmp658,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));}static struct
_dyneither_ptr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv);static struct
_dyneither_ptr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv){return tv->name;}
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs);struct Cyc_PP_Doc*
Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);{const char*_tmp65B;const char*_tmp65A;const char*
_tmp659;return Cyc_PP_egroup(((_tmp659="<",_tag_dyneither(_tmp659,sizeof(char),2))),((
_tmp65A=">",_tag_dyneither(_tmp65A,sizeof(char),2))),((_tmp65B=",",
_tag_dyneither(_tmp65B,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((
struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,tvs)));};}struct _tuple15{
struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct
_tuple15*t);struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple15*t){return Cyc_Absynpp_tqtd2doc((*
t).f1,(*t).f2,0);}struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts);
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){const char*_tmp65E;
const char*_tmp65D;const char*_tmp65C;return Cyc_PP_group(((_tmp65C="(",
_tag_dyneither(_tmp65C,sizeof(char),2))),((_tmp65D=")",_tag_dyneither(_tmp65D,
sizeof(char),2))),((_tmp65E=",",_tag_dyneither(_tmp65E,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*
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
if(_tmpA0->tag != 1)goto _LL62;}_LL61: {const char*_tmp65F;return Cyc_PP_text(((
_tmp65F=" _stdcall ",_tag_dyneither(_tmp65F,sizeof(char),11))));}_LL62: {struct
Cyc_Absyn_Cdecl_att_struct*_tmpA1=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp9F;if(
_tmpA1->tag != 2)goto _LL64;}_LL63: {const char*_tmp660;return Cyc_PP_text(((_tmp660=" _cdecl ",
_tag_dyneither(_tmp660,sizeof(char),9))));}_LL64: {struct Cyc_Absyn_Fastcall_att_struct*
_tmpA2=(struct Cyc_Absyn_Fastcall_att_struct*)_tmp9F;if(_tmpA2->tag != 3)goto _LL66;}
_LL65: {const char*_tmp661;return Cyc_PP_text(((_tmp661=" _fastcall ",
_tag_dyneither(_tmp661,sizeof(char),12))));}_LL66:;_LL67: goto _LL5F;_LL5F:;}
return Cyc_PP_nil_doc();}struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*
atts);struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){int
hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){void*
_tmpA6=(void*)atts2->hd;_LL69: {struct Cyc_Absyn_Stdcall_att_struct*_tmpA7=(
struct Cyc_Absyn_Stdcall_att_struct*)_tmpA6;if(_tmpA7->tag != 1)goto _LL6B;}_LL6A:
goto _LL6C;_LL6B: {struct Cyc_Absyn_Cdecl_att_struct*_tmpA8=(struct Cyc_Absyn_Cdecl_att_struct*)
_tmpA6;if(_tmpA8->tag != 2)goto _LL6D;}_LL6C: goto _LL6E;_LL6D: {struct Cyc_Absyn_Fastcall_att_struct*
_tmpA9=(struct Cyc_Absyn_Fastcall_att_struct*)_tmpA6;if(_tmpA9->tag != 3)goto _LL6F;}
_LL6E: goto _LL68;_LL6F:;_LL70: hasatt=1;goto _LL68;_LL68:;}}if(!hasatt)return Cyc_PP_nil_doc();{
const char*_tmp66C;const char*_tmp66B;const char*_tmp66A;const char*_tmp669;const
char*_tmp668;struct Cyc_PP_Doc*_tmp667[3];return(_tmp667[2]=Cyc_PP_text(((_tmp668=")",
_tag_dyneither(_tmp668,sizeof(char),2)))),((_tmp667[1]=Cyc_PP_group(((_tmp66B="",
_tag_dyneither(_tmp66B,sizeof(char),1))),((_tmp66A="",_tag_dyneither(_tmp66A,
sizeof(char),1))),((_tmp669=" ",_tag_dyneither(_tmp669,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_noncallatt2doc,atts)),((_tmp667[0]=Cyc_PP_text(((_tmp66C=" __declspec(",
_tag_dyneither(_tmp66C,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp667,
sizeof(struct Cyc_PP_Doc*),3)))))));};}struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*
a);struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){return Cyc_PP_text(Cyc_Absyn_attribute2string(
a));}struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts);struct Cyc_PP_Doc*
Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){if(atts == 0)return Cyc_PP_nil_doc();
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL71: return Cyc_Absynpp_noncallconv2doc(
atts);case Cyc_Cyclone_Gcc_c: _LL72: {const char*_tmp675;const char*_tmp674;const
char*_tmp673;const char*_tmp672;struct Cyc_PP_Doc*_tmp671[2];return(_tmp671[1]=Cyc_PP_group(((
_tmp674="((",_tag_dyneither(_tmp674,sizeof(char),3))),((_tmp673="))",
_tag_dyneither(_tmp673,sizeof(char),3))),((_tmp672=",",_tag_dyneither(_tmp672,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts)),((_tmp671[0]=Cyc_PP_text(((
_tmp675=" __attribute__",_tag_dyneither(_tmp675,sizeof(char),15)))),Cyc_PP_cat(
_tag_dyneither(_tmp671,sizeof(struct Cyc_PP_Doc*),2)))));}}}int Cyc_Absynpp_next_is_pointer(
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
_tmp676;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((
_tmp676="{?}",_tag_dyneither(_tmp676,sizeof(char),4))),1);}else{const char*
_tmp677;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp677="?",
_tag_dyneither(_tmp677,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)_check_null(
Cyc_Absynpp_cache_question);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb();static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned int)Cyc_Absynpp_cache_lb)){if(Cyc_PP_tex_output){const char*
_tmp678;Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp678="{\\lb}",
_tag_dyneither(_tmp678,sizeof(char),6))),1);}else{const char*_tmp679;Cyc_Absynpp_cache_lb=(
struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp679="{",_tag_dyneither(_tmp679,sizeof(char),
2))));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}static struct
Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;static struct Cyc_PP_Doc*Cyc_Absynpp_rb();static
struct Cyc_PP_Doc*Cyc_Absynpp_rb(){if(!((unsigned int)Cyc_Absynpp_cache_rb)){if(
Cyc_PP_tex_output){const char*_tmp67A;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((
_tmp67A="{\\rb}",_tag_dyneither(_tmp67A,sizeof(char),6))),1);}else{const char*
_tmp67B;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp67B="}",
_tag_dyneither(_tmp67B,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)_check_null(
Cyc_Absynpp_cache_rb);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;static
struct Cyc_PP_Doc*Cyc_Absynpp_dollar();static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){if(Cyc_PP_tex_output){const char*
_tmp67C;Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp67C="\\$",
_tag_dyneither(_tmp67C,sizeof(char),3))),1);}else{const char*_tmp67D;Cyc_Absynpp_cache_dollar=(
struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp67D="$",_tag_dyneither(_tmp67D,sizeof(char),
2))));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}struct
Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*
ss);struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*
ss){struct Cyc_PP_Doc*_tmp67E[3];return(_tmp67E[2]=Cyc_Absynpp_rb(),((_tmp67E[1]=
Cyc_PP_seq(sep,ss),((_tmp67E[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(
_tmp67E,sizeof(struct Cyc_PP_Doc*),3)))))));}static void Cyc_Absynpp_print_tms(
struct Cyc_List_List*tms);static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){void*_tmpC1=(void*)tms->hd;void*_tmpC5;struct Cyc_List_List*
_tmpC7;_LL7F: {struct Cyc_Absyn_Carray_mod_struct*_tmpC2=(struct Cyc_Absyn_Carray_mod_struct*)
_tmpC1;if(_tmpC2->tag != 0)goto _LL81;}_LL80:{const char*_tmp681;void*_tmp680;(
_tmp680=0,Cyc_fprintf(Cyc_stderr,((_tmp681="Carray_mod ",_tag_dyneither(_tmp681,
sizeof(char),12))),_tag_dyneither(_tmp680,sizeof(void*),0)));}goto _LL7E;_LL81: {
struct Cyc_Absyn_ConstArray_mod_struct*_tmpC3=(struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpC1;if(_tmpC3->tag != 1)goto _LL83;}_LL82:{const char*_tmp684;void*_tmp683;(
_tmp683=0,Cyc_fprintf(Cyc_stderr,((_tmp684="ConstArray_mod ",_tag_dyneither(
_tmp684,sizeof(char),16))),_tag_dyneither(_tmp683,sizeof(void*),0)));}goto _LL7E;
_LL83: {struct Cyc_Absyn_Function_mod_struct*_tmpC4=(struct Cyc_Absyn_Function_mod_struct*)
_tmpC1;if(_tmpC4->tag != 3)goto _LL85;else{_tmpC5=(void*)_tmpC4->f1;{struct Cyc_Absyn_WithTypes_struct*
_tmpC6=(struct Cyc_Absyn_WithTypes_struct*)_tmpC5;if(_tmpC6->tag != 1)goto _LL85;
else{_tmpC7=_tmpC6->f1;}};}}_LL84:{const char*_tmp687;void*_tmp686;(_tmp686=0,Cyc_fprintf(
Cyc_stderr,((_tmp687="Function_mod(",_tag_dyneither(_tmp687,sizeof(char),14))),
_tag_dyneither(_tmp686,sizeof(void*),0)));}for(0;_tmpC7 != 0;_tmpC7=_tmpC7->tl){
struct Cyc_Core_Opt*_tmpD2=(*((struct _tuple7*)_tmpC7->hd)).f1;if(_tmpD2 == 0){
const char*_tmp68A;void*_tmp689;(_tmp689=0,Cyc_fprintf(Cyc_stderr,((_tmp68A="?",
_tag_dyneither(_tmp68A,sizeof(char),2))),_tag_dyneither(_tmp689,sizeof(void*),0)));}
else{void*_tmp68B;(_tmp68B=0,Cyc_fprintf(Cyc_stderr,*((struct _dyneither_ptr*)
_tmpD2->v),_tag_dyneither(_tmp68B,sizeof(void*),0)));}if(_tmpC7->tl != 0){const
char*_tmp68E;void*_tmp68D;(_tmp68D=0,Cyc_fprintf(Cyc_stderr,((_tmp68E=",",
_tag_dyneither(_tmp68E,sizeof(char),2))),_tag_dyneither(_tmp68D,sizeof(void*),0)));}}{
const char*_tmp691;void*_tmp690;(_tmp690=0,Cyc_fprintf(Cyc_stderr,((_tmp691=") ",
_tag_dyneither(_tmp691,sizeof(char),3))),_tag_dyneither(_tmp690,sizeof(void*),0)));}
goto _LL7E;_LL85: {struct Cyc_Absyn_Function_mod_struct*_tmpC8=(struct Cyc_Absyn_Function_mod_struct*)
_tmpC1;if(_tmpC8->tag != 3)goto _LL87;}_LL86:{const char*_tmp694;void*_tmp693;(
_tmp693=0,Cyc_fprintf(Cyc_stderr,((_tmp694="Function_mod()",_tag_dyneither(
_tmp694,sizeof(char),15))),_tag_dyneither(_tmp693,sizeof(void*),0)));}goto _LL7E;
_LL87: {struct Cyc_Absyn_Attributes_mod_struct*_tmpC9=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpC1;if(_tmpC9->tag != 5)goto _LL89;}_LL88:{const char*_tmp697;void*_tmp696;(
_tmp696=0,Cyc_fprintf(Cyc_stderr,((_tmp697="Attributes_mod ",_tag_dyneither(
_tmp697,sizeof(char),16))),_tag_dyneither(_tmp696,sizeof(void*),0)));}goto _LL7E;
_LL89: {struct Cyc_Absyn_TypeParams_mod_struct*_tmpCA=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpC1;if(_tmpCA->tag != 4)goto _LL8B;}_LL8A:{const char*_tmp69A;void*_tmp699;(
_tmp699=0,Cyc_fprintf(Cyc_stderr,((_tmp69A="TypeParams_mod ",_tag_dyneither(
_tmp69A,sizeof(char),16))),_tag_dyneither(_tmp699,sizeof(void*),0)));}goto _LL7E;
_LL8B: {struct Cyc_Absyn_Pointer_mod_struct*_tmpCB=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmpC1;if(_tmpCB->tag != 2)goto _LL7E;}_LL8C:{const char*_tmp69D;void*_tmp69C;(
_tmp69C=0,Cyc_fprintf(Cyc_stderr,((_tmp69D="Pointer_mod ",_tag_dyneither(_tmp69D,
sizeof(char),13))),_tag_dyneither(_tmp69C,sizeof(void*),0)));}goto _LL7E;_LL7E:;}{
const char*_tmp6A0;void*_tmp69F;(_tmp69F=0,Cyc_fprintf(Cyc_stderr,((_tmp6A0="\n",
_tag_dyneither(_tmp6A0,sizeof(char),2))),_tag_dyneither(_tmp69F,sizeof(void*),0)));};}
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct
Cyc_List_List*tms);struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct
Cyc_PP_Doc*d,struct Cyc_List_List*tms){if(tms == 0)return d;{struct Cyc_PP_Doc*rest=
Cyc_Absynpp_dtms2doc(0,d,tms->tl);const char*_tmp6A5;const char*_tmp6A4;struct Cyc_PP_Doc*
_tmp6A3[3];struct Cyc_PP_Doc*p_rest=(_tmp6A3[2]=Cyc_PP_text(((_tmp6A4=")",
_tag_dyneither(_tmp6A4,sizeof(char),2)))),((_tmp6A3[1]=rest,((_tmp6A3[0]=Cyc_PP_text(((
_tmp6A5="(",_tag_dyneither(_tmp6A5,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp6A3,sizeof(struct Cyc_PP_Doc*),3)))))));void*_tmpE4=(void*)tms->hd;union Cyc_Absyn_Constraint*
_tmpE6;struct Cyc_Absyn_Exp*_tmpE8;union Cyc_Absyn_Constraint*_tmpE9;void*_tmpEB;
struct Cyc_List_List*_tmpED;struct Cyc_List_List*_tmpEF;struct Cyc_Position_Segment*
_tmpF0;int _tmpF1;struct Cyc_Absyn_PtrAtts _tmpF3;void*_tmpF4;union Cyc_Absyn_Constraint*
_tmpF5;union Cyc_Absyn_Constraint*_tmpF6;union Cyc_Absyn_Constraint*_tmpF7;struct
Cyc_Absyn_Tqual _tmpF8;_LL8E: {struct Cyc_Absyn_Carray_mod_struct*_tmpE5=(struct
Cyc_Absyn_Carray_mod_struct*)_tmpE4;if(_tmpE5->tag != 0)goto _LL90;else{_tmpE6=
_tmpE5->f1;}}_LL8F: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{const
char*_tmp6AA;const char*_tmp6A9;struct Cyc_PP_Doc*_tmp6A8[2];return(_tmp6A8[1]=((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpE6)?Cyc_PP_text(((
_tmp6A9="[]@zeroterm",_tag_dyneither(_tmp6A9,sizeof(char),12)))): Cyc_PP_text(((
_tmp6AA="[]",_tag_dyneither(_tmp6AA,sizeof(char),3)))),((_tmp6A8[0]=rest,Cyc_PP_cat(
_tag_dyneither(_tmp6A8,sizeof(struct Cyc_PP_Doc*),2)))));};_LL90: {struct Cyc_Absyn_ConstArray_mod_struct*
_tmpE7=(struct Cyc_Absyn_ConstArray_mod_struct*)_tmpE4;if(_tmpE7->tag != 1)goto
_LL92;else{_tmpE8=_tmpE7->f1;_tmpE9=_tmpE7->f2;}}_LL91: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{const char*_tmp6B1;const char*_tmp6B0;const char*_tmp6AF;
struct Cyc_PP_Doc*_tmp6AE[4];return(_tmp6AE[3]=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmpE9)?Cyc_PP_text(((_tmp6AF="]@zeroterm",
_tag_dyneither(_tmp6AF,sizeof(char),11)))): Cyc_PP_text(((_tmp6B0="]",
_tag_dyneither(_tmp6B0,sizeof(char),2)))),((_tmp6AE[2]=Cyc_Absynpp_exp2doc(
_tmpE8),((_tmp6AE[1]=Cyc_PP_text(((_tmp6B1="[",_tag_dyneither(_tmp6B1,sizeof(
char),2)))),((_tmp6AE[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6AE,sizeof(struct Cyc_PP_Doc*),
4)))))))));};_LL92: {struct Cyc_Absyn_Function_mod_struct*_tmpEA=(struct Cyc_Absyn_Function_mod_struct*)
_tmpE4;if(_tmpEA->tag != 3)goto _LL94;else{_tmpEB=(void*)_tmpEA->f1;}}_LL93: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{void*_tmp100=_tmpEB;struct Cyc_List_List*_tmp102;int _tmp103;
struct Cyc_Absyn_VarargInfo*_tmp104;struct Cyc_Core_Opt*_tmp105;struct Cyc_List_List*
_tmp106;struct Cyc_List_List*_tmp108;struct Cyc_Position_Segment*_tmp109;_LL9B: {
struct Cyc_Absyn_WithTypes_struct*_tmp101=(struct Cyc_Absyn_WithTypes_struct*)
_tmp100;if(_tmp101->tag != 1)goto _LL9D;else{_tmp102=_tmp101->f1;_tmp103=_tmp101->f2;
_tmp104=_tmp101->f3;_tmp105=_tmp101->f4;_tmp106=_tmp101->f5;}}_LL9C: {struct Cyc_PP_Doc*
_tmp6B2[2];return(_tmp6B2[1]=Cyc_Absynpp_funargs2doc(_tmp102,_tmp103,_tmp104,
_tmp105,_tmp106),((_tmp6B2[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6B2,sizeof(
struct Cyc_PP_Doc*),2)))));}_LL9D: {struct Cyc_Absyn_NoTypes_struct*_tmp107=(
struct Cyc_Absyn_NoTypes_struct*)_tmp100;if(_tmp107->tag != 0)goto _LL9A;else{
_tmp108=_tmp107->f1;_tmp109=_tmp107->f2;}}_LL9E: {const char*_tmp6B9;const char*
_tmp6B8;const char*_tmp6B7;struct Cyc_PP_Doc*_tmp6B6[2];return(_tmp6B6[1]=Cyc_PP_group(((
_tmp6B9="(",_tag_dyneither(_tmp6B9,sizeof(char),2))),((_tmp6B8=")",
_tag_dyneither(_tmp6B8,sizeof(char),2))),((_tmp6B7=",",_tag_dyneither(_tmp6B7,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp108)),((
_tmp6B6[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6B6,sizeof(struct Cyc_PP_Doc*),2)))));}
_LL9A:;};_LL94: {struct Cyc_Absyn_Attributes_mod_struct*_tmpEC=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpE4;if(_tmpEC->tag != 5)goto _LL96;else{_tmpED=_tmpEC->f2;}}_LL95: switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LL9F: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
struct Cyc_PP_Doc*_tmp6BA[2];return(_tmp6BA[1]=Cyc_Absynpp_atts2doc(_tmpED),((
_tmp6BA[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6BA,sizeof(struct Cyc_PP_Doc*),2)))));};
case Cyc_Cyclone_Vc_c: _LLA0: if(Cyc_Absynpp_next_is_pointer(tms->tl)){struct Cyc_PP_Doc*
_tmp6BB[2];return(_tmp6BB[1]=rest,((_tmp6BB[0]=Cyc_Absynpp_callconv2doc(_tmpED),
Cyc_PP_cat(_tag_dyneither(_tmp6BB,sizeof(struct Cyc_PP_Doc*),2)))));}return rest;}
_LL96: {struct Cyc_Absyn_TypeParams_mod_struct*_tmpEE=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpE4;if(_tmpEE->tag != 4)goto _LL98;else{_tmpEF=_tmpEE->f1;_tmpF0=_tmpEE->f2;
_tmpF1=_tmpEE->f3;}}_LL97: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmpF1){struct Cyc_PP_Doc*_tmp6BC[2];return(_tmp6BC[1]=Cyc_Absynpp_ktvars2doc(
_tmpEF),((_tmp6BC[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6BC,sizeof(struct Cyc_PP_Doc*),
2)))));}else{struct Cyc_PP_Doc*_tmp6BD[2];return(_tmp6BD[1]=Cyc_Absynpp_tvars2doc(
_tmpEF),((_tmp6BD[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6BD,sizeof(struct Cyc_PP_Doc*),
2)))));}_LL98: {struct Cyc_Absyn_Pointer_mod_struct*_tmpF2=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmpE4;if(_tmpF2->tag != 2)goto _LL8D;else{_tmpF3=_tmpF2->f1;_tmpF4=_tmpF3.rgn;
_tmpF5=_tmpF3.nullable;_tmpF6=_tmpF3.bounds;_tmpF7=_tmpF3.zero_term;_tmpF8=
_tmpF2->f2;}}_LL99: {struct Cyc_PP_Doc*ptr;{void*_tmp113=((void*(*)(void*y,union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmpF6);
struct Cyc_Absyn_Exp*_tmp116;_LLA3: {struct Cyc_Absyn_DynEither_b_struct*_tmp114=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp113;if(_tmp114->tag != 0)goto _LLA5;}_LLA4:
ptr=Cyc_Absynpp_question();goto _LLA2;_LLA5: {struct Cyc_Absyn_Upper_b_struct*
_tmp115=(struct Cyc_Absyn_Upper_b_struct*)_tmp113;if(_tmp115->tag != 1)goto _LLA2;
else{_tmp116=_tmp115->f1;}}_LLA6:{const char*_tmp6BF;const char*_tmp6BE;ptr=Cyc_PP_text(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmpF5)?(
_tmp6BF="*",_tag_dyneither(_tmp6BF,sizeof(char),2)):((_tmp6BE="@",_tag_dyneither(
_tmp6BE,sizeof(char),2))));}{unsigned int _tmp11A;int _tmp11B;struct _tuple10
_tmp119=Cyc_Evexp_eval_const_uint_exp(_tmp116);_tmp11A=_tmp119.f1;_tmp11B=
_tmp119.f2;if(!_tmp11B  || _tmp11A != 1){struct Cyc_PP_Doc*_tmp6C0[4];ptr=((_tmp6C0[
3]=Cyc_Absynpp_rb(),((_tmp6C0[2]=Cyc_Absynpp_exp2doc(_tmp116),((_tmp6C0[1]=Cyc_Absynpp_lb(),((
_tmp6C0[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6C0,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LLA2;};_LLA2:;}if(Cyc_Absynpp_print_zeroterm){if(!is_char_ptr  && ((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF7)){const char*
_tmp6C3;struct Cyc_PP_Doc*_tmp6C2[2];ptr=((_tmp6C2[1]=Cyc_PP_text(((_tmp6C3="@zeroterm ",
_tag_dyneither(_tmp6C3,sizeof(char),11)))),((_tmp6C2[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp6C2,sizeof(struct Cyc_PP_Doc*),2))))));}else{if(is_char_ptr
 && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF7)){
const char*_tmp6C6;struct Cyc_PP_Doc*_tmp6C5[2];ptr=((_tmp6C5[1]=Cyc_PP_text(((
_tmp6C6="@nozeroterm ",_tag_dyneither(_tmp6C6,sizeof(char),13)))),((_tmp6C5[0]=
ptr,Cyc_PP_cat(_tag_dyneither(_tmp6C5,sizeof(struct Cyc_PP_Doc*),2))))));}}}{void*
_tmp121=Cyc_Tcutil_compress(_tmpF4);_LLA8: {struct Cyc_Absyn_HeapRgn_struct*
_tmp122=(struct Cyc_Absyn_HeapRgn_struct*)_tmp121;if(_tmp122->tag != 21)goto _LLAA;}
_LLA9: goto _LLA7;_LLAA:{struct Cyc_Absyn_Evar_struct*_tmp123=(struct Cyc_Absyn_Evar_struct*)
_tmp121;if(_tmp123->tag != 1)goto _LLAC;}if(!Cyc_Absynpp_print_for_cycdoc)goto
_LLAC;_LLAB: goto _LLA7;_LLAC:;_LLAD: {const char*_tmp6C9;struct Cyc_PP_Doc*_tmp6C8[
3];ptr=((_tmp6C8[2]=Cyc_PP_text(((_tmp6C9=" ",_tag_dyneither(_tmp6C9,sizeof(char),
2)))),((_tmp6C8[1]=Cyc_Absynpp_typ2doc(_tmpF4),((_tmp6C8[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp6C8,sizeof(struct Cyc_PP_Doc*),3))))))));}_LLA7:;}{struct Cyc_PP_Doc*
_tmp6CA[2];ptr=((_tmp6CA[1]=Cyc_Absynpp_tqual2doc(_tmpF8),((_tmp6CA[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp6CA,sizeof(struct Cyc_PP_Doc*),2))))));}{struct Cyc_PP_Doc*
_tmp6CB[2];return(_tmp6CB[1]=rest,((_tmp6CB[0]=ptr,Cyc_PP_cat(_tag_dyneither(
_tmp6CB,sizeof(struct Cyc_PP_Doc*),2)))));};}_LL8D:;};}struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(
void*t);struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){void*_tmp12B=Cyc_Tcutil_compress(
t);_LLAF: {struct Cyc_Absyn_HeapRgn_struct*_tmp12C=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp12B;if(_tmp12C->tag != 21)goto _LLB1;}_LLB0: {const char*_tmp6CC;return Cyc_PP_text(((
_tmp6CC="`H",_tag_dyneither(_tmp6CC,sizeof(char),3))));}_LLB1: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp12D=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp12B;if(_tmp12D->tag != 22)goto
_LLB3;}_LLB2: {const char*_tmp6CD;return Cyc_PP_text(((_tmp6CD="`U",_tag_dyneither(
_tmp6CD,sizeof(char),3))));}_LLB3:;_LLB4: return Cyc_Absynpp_ntyp2doc(t);_LLAE:;}
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**
effects,void*t);static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,
struct Cyc_List_List**effects,void*t){void*_tmp130=Cyc_Tcutil_compress(t);void*
_tmp132;struct Cyc_List_List*_tmp134;_LLB6: {struct Cyc_Absyn_AccessEff_struct*
_tmp131=(struct Cyc_Absyn_AccessEff_struct*)_tmp130;if(_tmp131->tag != 23)goto
_LLB8;else{_tmp132=(void*)_tmp131->f1;}}_LLB7:{struct Cyc_List_List*_tmp6CE;*
rgions=((_tmp6CE=_cycalloc(sizeof(*_tmp6CE)),((_tmp6CE->hd=Cyc_Absynpp_rgn2doc(
_tmp132),((_tmp6CE->tl=*rgions,_tmp6CE))))));}goto _LLB5;_LLB8: {struct Cyc_Absyn_JoinEff_struct*
_tmp133=(struct Cyc_Absyn_JoinEff_struct*)_tmp130;if(_tmp133->tag != 24)goto _LLBA;
else{_tmp134=_tmp133->f1;}}_LLB9: for(0;_tmp134 != 0;_tmp134=_tmp134->tl){Cyc_Absynpp_effects2docs(
rgions,effects,(void*)_tmp134->hd);}goto _LLB5;_LLBA:;_LLBB:{struct Cyc_List_List*
_tmp6CF;*effects=((_tmp6CF=_cycalloc(sizeof(*_tmp6CF)),((_tmp6CF->hd=Cyc_Absynpp_typ2doc(
t),((_tmp6CF->tl=*effects,_tmp6CF))))));}goto _LLB5;_LLB5:;}struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(
void*t);struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){struct Cyc_List_List*rgions=
0;struct Cyc_List_List*effects=0;Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
effects);if(rgions == 0  && effects != 0){const char*_tmp6D2;const char*_tmp6D1;const
char*_tmp6D0;return Cyc_PP_group(((_tmp6D0="",_tag_dyneither(_tmp6D0,sizeof(char),
1))),((_tmp6D1="",_tag_dyneither(_tmp6D1,sizeof(char),1))),((_tmp6D2="+",
_tag_dyneither(_tmp6D2,sizeof(char),2))),effects);}else{const char*_tmp6D3;struct
Cyc_PP_Doc*_tmp13A=Cyc_Absynpp_group_braces(((_tmp6D3=",",_tag_dyneither(_tmp6D3,
sizeof(char),2))),rgions);struct Cyc_List_List*_tmp6D7;const char*_tmp6D6;const
char*_tmp6D5;const char*_tmp6D4;return Cyc_PP_group(((_tmp6D4="",_tag_dyneither(
_tmp6D4,sizeof(char),1))),((_tmp6D5="",_tag_dyneither(_tmp6D5,sizeof(char),1))),((
_tmp6D6="+",_tag_dyneither(_tmp6D6,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,((
_tmp6D7=_cycalloc(sizeof(*_tmp6D7)),((_tmp6D7->hd=_tmp13A,((_tmp6D7->tl=0,
_tmp6D7))))))));}}struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind
k);struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){switch(k){
case Cyc_Absyn_StructA: _LLBC: {const char*_tmp6D8;return Cyc_PP_text(((_tmp6D8="struct ",
_tag_dyneither(_tmp6D8,sizeof(char),8))));}case Cyc_Absyn_UnionA: _LLBD: {const
char*_tmp6D9;return Cyc_PP_text(((_tmp6D9="union ",_tag_dyneither(_tmp6D9,sizeof(
char),7))));}}}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);struct Cyc_PP_Doc*
Cyc_Absynpp_ntyp2doc(void*t){struct Cyc_PP_Doc*s;{void*_tmp142=t;struct Cyc_Core_Opt*
_tmp148;struct Cyc_Core_Opt*_tmp149;int _tmp14A;struct Cyc_Core_Opt*_tmp14B;struct
Cyc_Absyn_Tvar*_tmp14D;struct Cyc_Absyn_DatatypeInfo _tmp14F;union Cyc_Absyn_DatatypeInfoU
_tmp150;struct Cyc_List_List*_tmp151;struct Cyc_Absyn_DatatypeFieldInfo _tmp153;
union Cyc_Absyn_DatatypeFieldInfoU _tmp154;struct Cyc_List_List*_tmp155;enum Cyc_Absyn_Sign
_tmp157;enum Cyc_Absyn_Size_of _tmp158;int _tmp15B;struct Cyc_List_List*_tmp15D;
struct Cyc_Absyn_AggrInfo _tmp15F;union Cyc_Absyn_AggrInfoU _tmp160;struct Cyc_List_List*
_tmp161;enum Cyc_Absyn_AggrKind _tmp163;struct Cyc_List_List*_tmp164;struct Cyc_List_List*
_tmp166;struct _tuple0*_tmp168;struct Cyc_Absyn_Exp*_tmp16A;struct _tuple0*_tmp16C;
struct Cyc_List_List*_tmp16D;struct Cyc_Absyn_Typedefdecl*_tmp16E;void*_tmp170;
void*_tmp172;void*_tmp173;void*_tmp175;void*_tmp179;_LLC0: {struct Cyc_Absyn_ArrayType_struct*
_tmp143=(struct Cyc_Absyn_ArrayType_struct*)_tmp142;if(_tmp143->tag != 9)goto _LLC2;}
_LLC1: {const char*_tmp6DA;return Cyc_PP_text(((_tmp6DA="[[[array]]]",
_tag_dyneither(_tmp6DA,sizeof(char),12))));}_LLC2: {struct Cyc_Absyn_FnType_struct*
_tmp144=(struct Cyc_Absyn_FnType_struct*)_tmp142;if(_tmp144->tag != 10)goto _LLC4;}
_LLC3: return Cyc_PP_nil_doc();_LLC4: {struct Cyc_Absyn_PointerType_struct*_tmp145=(
struct Cyc_Absyn_PointerType_struct*)_tmp142;if(_tmp145->tag != 5)goto _LLC6;}_LLC5:
return Cyc_PP_nil_doc();_LLC6: {struct Cyc_Absyn_VoidType_struct*_tmp146=(struct
Cyc_Absyn_VoidType_struct*)_tmp142;if(_tmp146->tag != 0)goto _LLC8;}_LLC7:{const
char*_tmp6DB;s=Cyc_PP_text(((_tmp6DB="void",_tag_dyneither(_tmp6DB,sizeof(char),
5))));}goto _LLBF;_LLC8: {struct Cyc_Absyn_Evar_struct*_tmp147=(struct Cyc_Absyn_Evar_struct*)
_tmp142;if(_tmp147->tag != 1)goto _LLCA;else{_tmp148=_tmp147->f1;_tmp149=_tmp147->f2;
_tmp14A=_tmp147->f3;_tmp14B=_tmp147->f4;}}_LLC9: if(_tmp149 != 0)return Cyc_Absynpp_ntyp2doc((
void*)_tmp149->v);else{const char*_tmp6EB;const char*_tmp6EA;const char*_tmp6E9;
struct Cyc_Int_pa_struct _tmp6E8;void*_tmp6E7[1];const char*_tmp6E6;const char*
_tmp6E5;struct Cyc_PP_Doc*_tmp6E4[6];s=((_tmp6E4[5]=_tmp148 == 0?Cyc_Absynpp_question():
Cyc_Absynpp_kind2doc((enum Cyc_Absyn_Kind)_tmp148->v),((_tmp6E4[4]=Cyc_PP_text(((
_tmp6E5=")::",_tag_dyneither(_tmp6E5,sizeof(char),4)))),((_tmp6E4[3]=(!Cyc_Absynpp_print_full_evars
 || _tmp14B == 0)?Cyc_PP_nil_doc(): Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)
_tmp14B->v),((_tmp6E4[2]=Cyc_PP_text((struct _dyneither_ptr)((_tmp6E8.tag=1,((
_tmp6E8.f1=(unsigned long)_tmp14A,((_tmp6E7[0]=& _tmp6E8,Cyc_aprintf(((_tmp6E6="%d",
_tag_dyneither(_tmp6E6,sizeof(char),3))),_tag_dyneither(_tmp6E7,sizeof(void*),1))))))))),((
_tmp6E4[1]=Cyc_PP_text(((_tmp6E9="(",_tag_dyneither(_tmp6E9,sizeof(char),2)))),((
_tmp6E4[0]=Cyc_Absynpp_print_for_cycdoc?Cyc_PP_text(((_tmp6EA="\\%",
_tag_dyneither(_tmp6EA,sizeof(char),3)))): Cyc_PP_text(((_tmp6EB="%",
_tag_dyneither(_tmp6EB,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6E4,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));}goto _LLBF;_LLCA: {struct Cyc_Absyn_VarType_struct*
_tmp14C=(struct Cyc_Absyn_VarType_struct*)_tmp142;if(_tmp14C->tag != 2)goto _LLCC;
else{_tmp14D=_tmp14C->f1;}}_LLCB: s=Cyc_PP_textptr(_tmp14D->name);if(Cyc_Absynpp_print_all_kinds){
const char*_tmp6EE;struct Cyc_PP_Doc*_tmp6ED[3];s=((_tmp6ED[2]=Cyc_Absynpp_kindbound2doc(
_tmp14D->kind),((_tmp6ED[1]=Cyc_PP_text(((_tmp6EE="::",_tag_dyneither(_tmp6EE,
sizeof(char),3)))),((_tmp6ED[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp6ED,sizeof(
struct Cyc_PP_Doc*),3))))))));}if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(
_tmp14D)){const char*_tmp6F3;const char*_tmp6F2;struct Cyc_PP_Doc*_tmp6F1[3];s=((
_tmp6F1[2]=Cyc_PP_text(((_tmp6F2=" */",_tag_dyneither(_tmp6F2,sizeof(char),4)))),((
_tmp6F1[1]=s,((_tmp6F1[0]=Cyc_PP_text(((_tmp6F3="_ /* ",_tag_dyneither(_tmp6F3,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp6F1,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLBF;_LLCC: {struct Cyc_Absyn_DatatypeType_struct*_tmp14E=(struct Cyc_Absyn_DatatypeType_struct*)
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
_LLF8: {const char*_tmp6F4;struct Cyc_PP_Doc*_tmp194=Cyc_PP_text(((_tmp6F4="datatype ",
_tag_dyneither(_tmp6F4,sizeof(char),10))));const char*_tmp6F5;struct Cyc_PP_Doc*
_tmp195=_tmp193?Cyc_PP_text(((_tmp6F5="@extensible ",_tag_dyneither(_tmp6F5,
sizeof(char),13)))): Cyc_PP_nil_doc();{struct Cyc_PP_Doc*_tmp6F6[4];s=((_tmp6F6[3]=
Cyc_Absynpp_tps2doc(_tmp151),((_tmp6F6[2]=Cyc_Absynpp_qvar2doc(_tmp192),((
_tmp6F6[1]=_tmp194,((_tmp6F6[0]=_tmp195,Cyc_PP_cat(_tag_dyneither(_tmp6F6,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLF4;}_LLF4:;}goto _LLBF;_LLCE: {
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp152=(struct Cyc_Absyn_DatatypeFieldType_struct*)
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
_tmp1A3=_tmp19E.f2;_tmp1A4=*_tmp1A3;_tmp1A5=_tmp1A4.name;_LLFD: {const char*
_tmp6F8;const char*_tmp6F7;struct Cyc_PP_Doc*_tmp1A6=Cyc_PP_text(_tmp1A2?(_tmp6F8="@extensible datatype ",
_tag_dyneither(_tmp6F8,sizeof(char),22)):((_tmp6F7="datatype ",_tag_dyneither(
_tmp6F7,sizeof(char),10))));{const char*_tmp6FB;struct Cyc_PP_Doc*_tmp6FA[4];s=((
_tmp6FA[3]=Cyc_Absynpp_qvar2doc(_tmp1A5),((_tmp6FA[2]=Cyc_PP_text(((_tmp6FB=".",
_tag_dyneither(_tmp6FB,sizeof(char),2)))),((_tmp6FA[1]=Cyc_Absynpp_qvar2doc(
_tmp1A1),((_tmp6FA[0]=_tmp1A6,Cyc_PP_cat(_tag_dyneither(_tmp6FA,sizeof(struct Cyc_PP_Doc*),
4))))))))));}goto _LLF9;}_LLF9:;}goto _LLBF;_LLD0: {struct Cyc_Absyn_IntType_struct*
_tmp156=(struct Cyc_Absyn_IntType_struct*)_tmp142;if(_tmp156->tag != 6)goto _LLD2;
else{_tmp157=_tmp156->f1;_tmp158=_tmp156->f2;}}_LLD1: {struct _dyneither_ptr sns;
struct _dyneither_ptr ts;switch(_tmp157){case Cyc_Absyn_None: _LLFE: goto _LLFF;case
Cyc_Absyn_Signed: _LLFF:{const char*_tmp6FC;sns=((_tmp6FC="",_tag_dyneither(
_tmp6FC,sizeof(char),1)));}break;case Cyc_Absyn_Unsigned: _LL100:{const char*
_tmp6FD;sns=((_tmp6FD="unsigned ",_tag_dyneither(_tmp6FD,sizeof(char),10)));}
break;}switch(_tmp158){case Cyc_Absyn_Char_sz: _LL102: switch(_tmp157){case Cyc_Absyn_None:
_LL104:{const char*_tmp6FE;sns=((_tmp6FE="",_tag_dyneither(_tmp6FE,sizeof(char),1)));}
break;case Cyc_Absyn_Signed: _LL105:{const char*_tmp6FF;sns=((_tmp6FF="signed ",
_tag_dyneither(_tmp6FF,sizeof(char),8)));}break;case Cyc_Absyn_Unsigned: _LL106:{
const char*_tmp700;sns=((_tmp700="unsigned ",_tag_dyneither(_tmp700,sizeof(char),
10)));}break;}{const char*_tmp701;ts=((_tmp701="char",_tag_dyneither(_tmp701,
sizeof(char),5)));}break;case Cyc_Absyn_Short_sz: _LL103:{const char*_tmp702;ts=((
_tmp702="short",_tag_dyneither(_tmp702,sizeof(char),6)));}break;case Cyc_Absyn_Int_sz:
_LL108:{const char*_tmp703;ts=((_tmp703="int",_tag_dyneither(_tmp703,sizeof(char),
4)));}break;case Cyc_Absyn_Long_sz: _LL109:{const char*_tmp704;ts=((_tmp704="long",
_tag_dyneither(_tmp704,sizeof(char),5)));}break;case Cyc_Absyn_LongLong_sz: _LL10A:
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL10C:{const char*_tmp705;
ts=((_tmp705="long long",_tag_dyneither(_tmp705,sizeof(char),10)));}break;case
Cyc_Cyclone_Vc_c: _LL10D:{const char*_tmp706;ts=((_tmp706="__int64",_tag_dyneither(
_tmp706,sizeof(char),8)));}break;}break;}{const char*_tmp70B;void*_tmp70A[2];
struct Cyc_String_pa_struct _tmp709;struct Cyc_String_pa_struct _tmp708;s=Cyc_PP_text((
struct _dyneither_ptr)((_tmp708.tag=0,((_tmp708.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)ts),((_tmp709.tag=0,((_tmp709.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)sns),((_tmp70A[0]=& _tmp709,((_tmp70A[1]=& _tmp708,Cyc_aprintf(((
_tmp70B="%s%s",_tag_dyneither(_tmp70B,sizeof(char),5))),_tag_dyneither(_tmp70A,
sizeof(void*),2)))))))))))))));}goto _LLBF;}_LLD2: {struct Cyc_Absyn_FloatType_struct*
_tmp159=(struct Cyc_Absyn_FloatType_struct*)_tmp142;if(_tmp159->tag != 7)goto _LLD4;}
_LLD3:{const char*_tmp70C;s=Cyc_PP_text(((_tmp70C="float",_tag_dyneither(_tmp70C,
sizeof(char),6))));}goto _LLBF;_LLD4: {struct Cyc_Absyn_DoubleType_struct*_tmp15A=(
struct Cyc_Absyn_DoubleType_struct*)_tmp142;if(_tmp15A->tag != 8)goto _LLD6;else{
_tmp15B=_tmp15A->f1;}}_LLD5: if(_tmp15B){const char*_tmp70D;s=Cyc_PP_text(((
_tmp70D="long double",_tag_dyneither(_tmp70D,sizeof(char),12))));}else{const char*
_tmp70E;s=Cyc_PP_text(((_tmp70E="double",_tag_dyneither(_tmp70E,sizeof(char),7))));}
goto _LLBF;_LLD6: {struct Cyc_Absyn_TupleType_struct*_tmp15C=(struct Cyc_Absyn_TupleType_struct*)
_tmp142;if(_tmp15C->tag != 11)goto _LLD8;else{_tmp15D=_tmp15C->f1;}}_LLD7:{struct
Cyc_PP_Doc*_tmp70F[2];s=((_tmp70F[1]=Cyc_Absynpp_args2doc(_tmp15D),((_tmp70F[0]=
Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp70F,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LLBF;_LLD8: {struct Cyc_Absyn_AggrType_struct*_tmp15E=(struct Cyc_Absyn_AggrType_struct*)
_tmp142;if(_tmp15E->tag != 12)goto _LLDA;else{_tmp15F=_tmp15E->f1;_tmp160=_tmp15F.aggr_info;
_tmp161=_tmp15F.targs;}}_LLD9: {enum Cyc_Absyn_AggrKind _tmp1BF;struct _tuple0*
_tmp1C0;struct _tuple9 _tmp1BE=Cyc_Absyn_aggr_kinded_name(_tmp160);_tmp1BF=_tmp1BE.f1;
_tmp1C0=_tmp1BE.f2;{struct Cyc_PP_Doc*_tmp710[3];s=((_tmp710[2]=Cyc_Absynpp_tps2doc(
_tmp161),((_tmp710[1]=Cyc_Absynpp_qvar2doc(_tmp1C0),((_tmp710[0]=Cyc_Absynpp_aggr_kind2doc(
_tmp1BF),Cyc_PP_cat(_tag_dyneither(_tmp710,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLBF;}_LLDA: {struct Cyc_Absyn_AnonAggrType_struct*_tmp162=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp142;if(_tmp162->tag != 13)goto _LLDC;else{_tmp163=_tmp162->f1;_tmp164=_tmp162->f2;}}
_LLDB:{struct Cyc_PP_Doc*_tmp711[4];s=((_tmp711[3]=Cyc_Absynpp_rb(),((_tmp711[2]=
Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp164)),((_tmp711[1]=Cyc_Absynpp_lb(),((
_tmp711[0]=Cyc_Absynpp_aggr_kind2doc(_tmp163),Cyc_PP_cat(_tag_dyneither(_tmp711,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLBF;_LLDC: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp165=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp142;if(_tmp165->tag != 15)goto
_LLDE;else{_tmp166=_tmp165->f1;}}_LLDD:{const char*_tmp714;struct Cyc_PP_Doc*
_tmp713[4];s=((_tmp713[3]=Cyc_Absynpp_rb(),((_tmp713[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(
_tmp166)),((_tmp713[1]=Cyc_Absynpp_lb(),((_tmp713[0]=Cyc_PP_text(((_tmp714="enum ",
_tag_dyneither(_tmp714,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp713,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLBF;_LLDE: {struct Cyc_Absyn_EnumType_struct*
_tmp167=(struct Cyc_Absyn_EnumType_struct*)_tmp142;if(_tmp167->tag != 14)goto _LLE0;
else{_tmp168=_tmp167->f1;}}_LLDF:{const char*_tmp717;struct Cyc_PP_Doc*_tmp716[2];
s=((_tmp716[1]=Cyc_Absynpp_qvar2doc(_tmp168),((_tmp716[0]=Cyc_PP_text(((_tmp717="enum ",
_tag_dyneither(_tmp717,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp716,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLBF;_LLE0: {struct Cyc_Absyn_ValueofType_struct*
_tmp169=(struct Cyc_Absyn_ValueofType_struct*)_tmp142;if(_tmp169->tag != 19)goto
_LLE2;else{_tmp16A=_tmp169->f1;}}_LLE1:{const char*_tmp71C;const char*_tmp71B;
struct Cyc_PP_Doc*_tmp71A[3];s=((_tmp71A[2]=Cyc_PP_text(((_tmp71B=")",
_tag_dyneither(_tmp71B,sizeof(char),2)))),((_tmp71A[1]=Cyc_Absynpp_exp2doc(
_tmp16A),((_tmp71A[0]=Cyc_PP_text(((_tmp71C="valueof_t(",_tag_dyneither(_tmp71C,
sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp71A,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LLBF;_LLE2: {struct Cyc_Absyn_TypedefType_struct*_tmp16B=(struct
Cyc_Absyn_TypedefType_struct*)_tmp142;if(_tmp16B->tag != 18)goto _LLE4;else{
_tmp16C=_tmp16B->f1;_tmp16D=_tmp16B->f2;_tmp16E=_tmp16B->f3;}}_LLE3:{struct Cyc_PP_Doc*
_tmp71D[2];s=((_tmp71D[1]=Cyc_Absynpp_tps2doc(_tmp16D),((_tmp71D[0]=Cyc_Absynpp_qvar2doc(
_tmp16C),Cyc_PP_cat(_tag_dyneither(_tmp71D,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLBF;_LLE4: {struct Cyc_Absyn_RgnHandleType_struct*_tmp16F=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp142;if(_tmp16F->tag != 16)goto _LLE6;else{_tmp170=(void*)_tmp16F->f1;}}_LLE5:{
const char*_tmp722;const char*_tmp721;struct Cyc_PP_Doc*_tmp720[3];s=((_tmp720[2]=
Cyc_PP_text(((_tmp721=">",_tag_dyneither(_tmp721,sizeof(char),2)))),((_tmp720[1]=
Cyc_Absynpp_rgn2doc(_tmp170),((_tmp720[0]=Cyc_PP_text(((_tmp722="region_t<",
_tag_dyneither(_tmp722,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp720,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLBF;_LLE6: {struct Cyc_Absyn_DynRgnType_struct*
_tmp171=(struct Cyc_Absyn_DynRgnType_struct*)_tmp142;if(_tmp171->tag != 17)goto
_LLE8;else{_tmp172=(void*)_tmp171->f1;_tmp173=(void*)_tmp171->f2;}}_LLE7:{const
char*_tmp729;const char*_tmp728;const char*_tmp727;struct Cyc_PP_Doc*_tmp726[5];s=((
_tmp726[4]=Cyc_PP_text(((_tmp727=">",_tag_dyneither(_tmp727,sizeof(char),2)))),((
_tmp726[3]=Cyc_Absynpp_rgn2doc(_tmp173),((_tmp726[2]=Cyc_PP_text(((_tmp728=",",
_tag_dyneither(_tmp728,sizeof(char),2)))),((_tmp726[1]=Cyc_Absynpp_rgn2doc(
_tmp172),((_tmp726[0]=Cyc_PP_text(((_tmp729="dynregion_t<",_tag_dyneither(
_tmp729,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp726,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LLBF;_LLE8: {struct Cyc_Absyn_TagType_struct*_tmp174=(struct
Cyc_Absyn_TagType_struct*)_tmp142;if(_tmp174->tag != 20)goto _LLEA;else{_tmp175=(
void*)_tmp174->f1;}}_LLE9:{const char*_tmp72E;const char*_tmp72D;struct Cyc_PP_Doc*
_tmp72C[3];s=((_tmp72C[2]=Cyc_PP_text(((_tmp72D=">",_tag_dyneither(_tmp72D,
sizeof(char),2)))),((_tmp72C[1]=Cyc_Absynpp_typ2doc(_tmp175),((_tmp72C[0]=Cyc_PP_text(((
_tmp72E="tag_t<",_tag_dyneither(_tmp72E,sizeof(char),7)))),Cyc_PP_cat(
_tag_dyneither(_tmp72C,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLBF;_LLEA: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp176=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp142;if(_tmp176->tag != 22)goto _LLEC;}_LLEB: goto _LLED;_LLEC: {struct Cyc_Absyn_HeapRgn_struct*
_tmp177=(struct Cyc_Absyn_HeapRgn_struct*)_tmp142;if(_tmp177->tag != 21)goto _LLEE;}
_LLED: s=Cyc_Absynpp_rgn2doc(t);goto _LLBF;_LLEE: {struct Cyc_Absyn_RgnsEff_struct*
_tmp178=(struct Cyc_Absyn_RgnsEff_struct*)_tmp142;if(_tmp178->tag != 25)goto _LLF0;
else{_tmp179=(void*)_tmp178->f1;}}_LLEF:{const char*_tmp733;const char*_tmp732;
struct Cyc_PP_Doc*_tmp731[3];s=((_tmp731[2]=Cyc_PP_text(((_tmp732=")",
_tag_dyneither(_tmp732,sizeof(char),2)))),((_tmp731[1]=Cyc_Absynpp_typ2doc(
_tmp179),((_tmp731[0]=Cyc_PP_text(((_tmp733="regions(",_tag_dyneither(_tmp733,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp731,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLBF;_LLF0: {struct Cyc_Absyn_AccessEff_struct*_tmp17A=(struct Cyc_Absyn_AccessEff_struct*)
_tmp142;if(_tmp17A->tag != 23)goto _LLF2;}_LLF1: goto _LLF3;_LLF2: {struct Cyc_Absyn_JoinEff_struct*
_tmp17B=(struct Cyc_Absyn_JoinEff_struct*)_tmp142;if(_tmp17B->tag != 24)goto _LLBF;}
_LLF3: s=Cyc_Absynpp_eff2doc(t);goto _LLBF;_LLBF:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(
struct Cyc_Core_Opt*vo);struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct Cyc_Core_Opt*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*((struct _dyneither_ptr*)vo->v));}
struct _tuple16{void*f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct
_tuple16*cmp);struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple16*cmp){
struct _tuple16 _tmp1D9;void*_tmp1DA;void*_tmp1DB;struct _tuple16*_tmp1D8=cmp;
_tmp1D9=*_tmp1D8;_tmp1DA=_tmp1D9.f1;_tmp1DB=_tmp1D9.f2;{const char*_tmp736;struct
Cyc_PP_Doc*_tmp735[3];return(_tmp735[2]=Cyc_Absynpp_rgn2doc(_tmp1DB),((_tmp735[1]=
Cyc_PP_text(((_tmp736=" > ",_tag_dyneither(_tmp736,sizeof(char),4)))),((_tmp735[
0]=Cyc_Absynpp_rgn2doc(_tmp1DA),Cyc_PP_cat(_tag_dyneither(_tmp735,sizeof(struct
Cyc_PP_Doc*),3)))))));};}struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*
po);struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){const char*
_tmp739;const char*_tmp738;const char*_tmp737;return Cyc_PP_group(((_tmp737="",
_tag_dyneither(_tmp737,sizeof(char),1))),((_tmp738="",_tag_dyneither(_tmp738,
sizeof(char),1))),((_tmp739=",",_tag_dyneither(_tmp739,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(
struct _tuple7*t);struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple7*t){struct
Cyc_Core_Opt*_tmp73A;struct Cyc_Core_Opt*dopt=(*t).f1 == 0?0:((_tmp73A=_cycalloc(
sizeof(*_tmp73A)),((_tmp73A->v=Cyc_PP_text(*((struct _dyneither_ptr*)((struct Cyc_Core_Opt*)
_check_null((*t).f1))->v)),_tmp73A))));return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,
dopt);}struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*effopt,
struct Cyc_List_List*rgn_po);struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*
effopt,struct Cyc_List_List*rgn_po){struct Cyc_List_List*_tmp1E2=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,
args);struct Cyc_PP_Doc*eff_doc;if(c_varargs){const char*_tmp73D;struct Cyc_List_List*
_tmp73C;_tmp1E2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp1E2,((_tmp73C=_cycalloc(sizeof(*_tmp73C)),((_tmp73C->hd=
Cyc_PP_text(((_tmp73D="...",_tag_dyneither(_tmp73D,sizeof(char),4)))),((_tmp73C->tl=
0,_tmp73C)))))));}else{if(cyc_varargs != 0){const char*_tmp746;const char*_tmp745;
const char*_tmp744;struct _tuple7*_tmp743;struct Cyc_PP_Doc*_tmp742[3];struct Cyc_PP_Doc*
_tmp1E5=(_tmp742[2]=Cyc_Absynpp_funarg2doc(((_tmp743=_cycalloc(sizeof(*_tmp743)),((
_tmp743->f1=cyc_varargs->name,((_tmp743->f2=cyc_varargs->tq,((_tmp743->f3=
cyc_varargs->type,_tmp743))))))))),((_tmp742[1]=cyc_varargs->inject?Cyc_PP_text(((
_tmp744=" inject ",_tag_dyneither(_tmp744,sizeof(char),9)))): Cyc_PP_text(((
_tmp745=" ",_tag_dyneither(_tmp745,sizeof(char),2)))),((_tmp742[0]=Cyc_PP_text(((
_tmp746="...",_tag_dyneither(_tmp746,sizeof(char),4)))),Cyc_PP_cat(
_tag_dyneither(_tmp742,sizeof(struct Cyc_PP_Doc*),3)))))));struct Cyc_List_List*
_tmp747;_tmp1E2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp1E2,((_tmp747=_cycalloc(sizeof(*_tmp747)),((_tmp747->hd=
_tmp1E5,((_tmp747->tl=0,_tmp747)))))));}}{const char*_tmp74A;const char*_tmp749;
const char*_tmp748;struct Cyc_PP_Doc*_tmp1EC=Cyc_PP_group(((_tmp748="",
_tag_dyneither(_tmp748,sizeof(char),1))),((_tmp749="",_tag_dyneither(_tmp749,
sizeof(char),1))),((_tmp74A=",",_tag_dyneither(_tmp74A,sizeof(char),2))),_tmp1E2);
if(effopt != 0  && Cyc_Absynpp_print_all_effects){const char*_tmp74D;struct Cyc_PP_Doc*
_tmp74C[3];_tmp1EC=((_tmp74C[2]=Cyc_Absynpp_eff2doc((void*)effopt->v),((_tmp74C[
1]=Cyc_PP_text(((_tmp74D=";",_tag_dyneither(_tmp74D,sizeof(char),2)))),((_tmp74C[
0]=_tmp1EC,Cyc_PP_cat(_tag_dyneither(_tmp74C,sizeof(struct Cyc_PP_Doc*),3))))))));}
if(rgn_po != 0){const char*_tmp750;struct Cyc_PP_Doc*_tmp74F[3];_tmp1EC=((_tmp74F[2]=
Cyc_Absynpp_rgnpo2doc(rgn_po),((_tmp74F[1]=Cyc_PP_text(((_tmp750=":",
_tag_dyneither(_tmp750,sizeof(char),2)))),((_tmp74F[0]=_tmp1EC,Cyc_PP_cat(
_tag_dyneither(_tmp74F,sizeof(struct Cyc_PP_Doc*),3))))))));}{const char*_tmp755;
const char*_tmp754;struct Cyc_PP_Doc*_tmp753[3];return(_tmp753[2]=Cyc_PP_text(((
_tmp754=")",_tag_dyneither(_tmp754,sizeof(char),2)))),((_tmp753[1]=_tmp1EC,((
_tmp753[0]=Cyc_PP_text(((_tmp755="(",_tag_dyneither(_tmp755,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp753,sizeof(struct Cyc_PP_Doc*),3)))))));};};}struct
_tuple7*Cyc_Absynpp_arg_mk_opt(struct _tuple11*arg);struct _tuple7*Cyc_Absynpp_arg_mk_opt(
struct _tuple11*arg){struct Cyc_Core_Opt*_tmp758;struct _tuple7*_tmp757;return(
_tmp757=_cycalloc(sizeof(*_tmp757)),((_tmp757->f1=((_tmp758=_cycalloc(sizeof(*
_tmp758)),((_tmp758->v=(*arg).f1,_tmp758)))),((_tmp757->f2=(*arg).f2,((_tmp757->f3=(*
arg).f3,_tmp757)))))));}struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct
_dyneither_ptr*v);struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dyneither_ptr*v){
return Cyc_PP_text(*v);}struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*
q);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){struct Cyc_List_List*
_tmp1F9=0;int match;{union Cyc_Absyn_Nmspace _tmp1FA=(*q).f1;int _tmp1FB;struct Cyc_List_List*
_tmp1FC;struct Cyc_List_List*_tmp1FD;_LL110: if((_tmp1FA.Loc_n).tag != 3)goto _LL112;
_tmp1FB=(int)(_tmp1FA.Loc_n).val;_LL111: _tmp1FC=0;goto _LL113;_LL112: if((_tmp1FA.Rel_n).tag
!= 1)goto _LL114;_tmp1FC=(struct Cyc_List_List*)(_tmp1FA.Rel_n).val;_LL113: match=0;
_tmp1F9=_tmp1FC;goto _LL10F;_LL114: if((_tmp1FA.Abs_n).tag != 2)goto _LL10F;_tmp1FD=(
struct Cyc_List_List*)(_tmp1FA.Abs_n).val;_LL115: match=Cyc_Absynpp_use_curr_namespace
 && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1FD,Cyc_Absynpp_curr_namespace);{
struct Cyc_List_List*_tmp759;_tmp1F9=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?(
_tmp759=_cycalloc(sizeof(*_tmp759)),((_tmp759->hd=Cyc_Absynpp_cyc_stringptr,((
_tmp759->tl=_tmp1FD,_tmp759))))): _tmp1FD;}goto _LL10F;_LL10F:;}if(Cyc_Absynpp_qvar_to_Cids){
const char*_tmp75B;struct Cyc_List_List*_tmp75A;return(struct _dyneither_ptr)Cyc_str_sepstr(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp1F9,((_tmp75A=_cycalloc(sizeof(*_tmp75A)),((_tmp75A->hd=(*q).f2,((_tmp75A->tl=
0,_tmp75A))))))),((_tmp75B="_",_tag_dyneither(_tmp75B,sizeof(char),2))));}else{
if(match)return*(*q).f2;else{const char*_tmp75D;struct Cyc_List_List*_tmp75C;
return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1F9,((_tmp75C=_cycalloc(sizeof(*
_tmp75C)),((_tmp75C->hd=(*q).f2,((_tmp75C->tl=0,_tmp75C))))))),((_tmp75D="::",
_tag_dyneither(_tmp75D,sizeof(char),3))));}}}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(
struct _tuple0*q);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){return
Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(
struct _tuple0*q);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _dyneither_ptr _tmp203=Cyc_Absynpp_qvar2string(q);if(Cyc_PP_tex_output){
const char*_tmp75F;const char*_tmp75E;return Cyc_PP_text_width((struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp75E="\\textbf{",
_tag_dyneither(_tmp75E,sizeof(char),9))),(struct _dyneither_ptr)_tmp203),((
_tmp75F="}",_tag_dyneither(_tmp75F,sizeof(char),2)))),(int)Cyc_strlen((struct
_dyneither_ptr)_tmp203));}else{return Cyc_PP_text(_tmp203);}}struct _dyneither_ptr
Cyc_Absynpp_typedef_name2string(struct _tuple0*v);struct _dyneither_ptr Cyc_Absynpp_typedef_name2string(
struct _tuple0*v){if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
if(Cyc_Absynpp_use_curr_namespace){union Cyc_Absyn_Nmspace _tmp206=(*v).f1;int
_tmp207;struct Cyc_List_List*_tmp208;struct Cyc_List_List*_tmp209;_LL117: if((
_tmp206.Loc_n).tag != 3)goto _LL119;_tmp207=(int)(_tmp206.Loc_n).val;_LL118: goto
_LL11A;_LL119: if((_tmp206.Rel_n).tag != 1)goto _LL11B;_tmp208=(struct Cyc_List_List*)(
_tmp206.Rel_n).val;if(_tmp208 != 0)goto _LL11B;_LL11A: return*(*v).f2;_LL11B: if((
_tmp206.Abs_n).tag != 2)goto _LL11D;_tmp209=(struct Cyc_List_List*)(_tmp206.Abs_n).val;
_LL11C: if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct
Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,
_tmp209,Cyc_Absynpp_curr_namespace)== 0)return*(*v).f2;else{goto _LL11E;}_LL11D:;
_LL11E: {const char*_tmp760;return(struct _dyneither_ptr)Cyc_strconcat(((_tmp760="/* bad namespace : */ ",
_tag_dyneither(_tmp760,sizeof(char),23))),(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
v));}_LL116:;}else{return*(*v).f2;}}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(
struct _tuple0*v);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(
struct _tuple0*v);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*
v){struct _dyneither_ptr _tmp20B=Cyc_Absynpp_typedef_name2string(v);if(Cyc_PP_tex_output){
const char*_tmp762;const char*_tmp761;return Cyc_PP_text_width((struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp761="\\textbf{",
_tag_dyneither(_tmp761,sizeof(char),9))),(struct _dyneither_ptr)_tmp20B),((
_tmp762="}",_tag_dyneither(_tmp762,sizeof(char),2)))),(int)Cyc_strlen((struct
_dyneither_ptr)_tmp20B));}else{return Cyc_PP_text(_tmp20B);}}struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(
void*t);struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){return Cyc_Absynpp_tqtd2doc(
Cyc_Absyn_empty_tqual(0),t,0);}int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e);
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){void*_tmp20E=e->r;enum Cyc_Absyn_Primop
_tmp213;struct Cyc_Absyn_Exp*_tmp21E;struct Cyc_Absyn_Exp*_tmp220;_LL120: {struct
Cyc_Absyn_Const_e_struct*_tmp20F=(struct Cyc_Absyn_Const_e_struct*)_tmp20E;if(
_tmp20F->tag != 0)goto _LL122;}_LL121: goto _LL123;_LL122: {struct Cyc_Absyn_Var_e_struct*
_tmp210=(struct Cyc_Absyn_Var_e_struct*)_tmp20E;if(_tmp210->tag != 1)goto _LL124;}
_LL123: goto _LL125;_LL124: {struct Cyc_Absyn_UnknownId_e_struct*_tmp211=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmp20E;if(_tmp211->tag != 2)goto _LL126;}_LL125: return 10000;_LL126: {struct Cyc_Absyn_Primop_e_struct*
_tmp212=(struct Cyc_Absyn_Primop_e_struct*)_tmp20E;if(_tmp212->tag != 3)goto _LL128;
else{_tmp213=_tmp212->f1;}}_LL127: switch(_tmp213){case Cyc_Absyn_Plus: _LL172:
return 100;case Cyc_Absyn_Times: _LL173: return 110;case Cyc_Absyn_Minus: _LL174: return
100;case Cyc_Absyn_Div: _LL175: goto _LL176;case Cyc_Absyn_Mod: _LL176: return 110;case
Cyc_Absyn_Eq: _LL177: goto _LL178;case Cyc_Absyn_Neq: _LL178: return 70;case Cyc_Absyn_Gt:
_LL179: goto _LL17A;case Cyc_Absyn_Lt: _LL17A: goto _LL17B;case Cyc_Absyn_Gte: _LL17B:
goto _LL17C;case Cyc_Absyn_Lte: _LL17C: return 80;case Cyc_Absyn_Not: _LL17D: goto _LL17E;
case Cyc_Absyn_Bitnot: _LL17E: return 130;case Cyc_Absyn_Bitand: _LL17F: return 60;case
Cyc_Absyn_Bitor: _LL180: return 40;case Cyc_Absyn_Bitxor: _LL181: return 50;case Cyc_Absyn_Bitlshift:
_LL182: return 90;case Cyc_Absyn_Bitlrshift: _LL183: return 80;case Cyc_Absyn_Bitarshift:
_LL184: return 80;case Cyc_Absyn_Numelts: _LL185: return 140;}_LL128: {struct Cyc_Absyn_AssignOp_e_struct*
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
_tmp226=(struct Cyc_Absyn_Valueof_e_struct*)_tmp20E;if(_tmp226->tag != 40)goto
_LL14A;}_LL149: goto _LL14B;_LL14A: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp227=(
struct Cyc_Absyn_Tagcheck_e_struct*)_tmp20E;if(_tmp227->tag != 39)goto _LL14C;}
_LL14B: goto _LL14D;_LL14C: {struct Cyc_Absyn_Offsetof_e_struct*_tmp228=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp20E;if(_tmp228->tag != 20)goto _LL14E;}_LL14D: goto _LL14F;_LL14E: {struct Cyc_Absyn_Gentyp_e_struct*
_tmp229=(struct Cyc_Absyn_Gentyp_e_struct*)_tmp20E;if(_tmp229->tag != 21)goto
_LL150;}_LL14F: goto _LL151;_LL150: {struct Cyc_Absyn_Deref_e_struct*_tmp22A=(
struct Cyc_Absyn_Deref_e_struct*)_tmp20E;if(_tmp22A->tag != 22)goto _LL152;}_LL151:
return 130;_LL152: {struct Cyc_Absyn_AggrMember_e_struct*_tmp22B=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp20E;if(_tmp22B->tag != 23)goto _LL154;}_LL153: goto _LL155;_LL154: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp22C=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp20E;if(_tmp22C->tag != 24)goto
_LL156;}_LL155: goto _LL157;_LL156: {struct Cyc_Absyn_Subscript_e_struct*_tmp22D=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp20E;if(_tmp22D->tag != 25)goto _LL158;}
_LL157: return 140;_LL158: {struct Cyc_Absyn_Tuple_e_struct*_tmp22E=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp20E;if(_tmp22E->tag != 26)goto _LL15A;}_LL159: return 150;_LL15A: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp22F=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp20E;if(_tmp22F->tag != 27)goto
_LL15C;}_LL15B: goto _LL15D;_LL15C: {struct Cyc_Absyn_Array_e_struct*_tmp230=(
struct Cyc_Absyn_Array_e_struct*)_tmp20E;if(_tmp230->tag != 28)goto _LL15E;}_LL15D:
goto _LL15F;_LL15E: {struct Cyc_Absyn_Comprehension_e_struct*_tmp231=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp20E;if(_tmp231->tag != 29)goto _LL160;}_LL15F: goto _LL161;_LL160: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp232=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp20E;if(_tmp232->tag != 30)goto
_LL162;}_LL161: goto _LL163;_LL162: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp233=(
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp20E;if(_tmp233->tag != 31)goto _LL164;}
_LL163: goto _LL165;_LL164: {struct Cyc_Absyn_Datatype_e_struct*_tmp234=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp20E;if(_tmp234->tag != 32)goto _LL166;}_LL165: goto _LL167;_LL166: {struct Cyc_Absyn_Enum_e_struct*
_tmp235=(struct Cyc_Absyn_Enum_e_struct*)_tmp20E;if(_tmp235->tag != 33)goto _LL168;}
_LL167: goto _LL169;_LL168: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp236=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp20E;if(_tmp236->tag != 34)goto _LL16A;}_LL169: goto _LL16B;_LL16A: {struct Cyc_Absyn_Malloc_e_struct*
_tmp237=(struct Cyc_Absyn_Malloc_e_struct*)_tmp20E;if(_tmp237->tag != 35)goto
_LL16C;}_LL16B: goto _LL16D;_LL16C: {struct Cyc_Absyn_Swap_e_struct*_tmp238=(struct
Cyc_Absyn_Swap_e_struct*)_tmp20E;if(_tmp238->tag != 36)goto _LL16E;}_LL16D: goto
_LL16F;_LL16E: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp239=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp20E;if(_tmp239->tag != 37)goto _LL170;}_LL16F: return 140;_LL170: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp23A=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp20E;if(_tmp23A->tag != 38)goto
_LL11F;}_LL171: return 10000;_LL11F:;}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct
Cyc_Absyn_Exp*e);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int
inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*
s;{void*_tmp23B=e->r;union Cyc_Absyn_Cnst _tmp23D;struct _tuple0*_tmp23F;struct
_tuple0*_tmp241;enum Cyc_Absyn_Primop _tmp243;struct Cyc_List_List*_tmp244;struct
Cyc_Absyn_Exp*_tmp246;struct Cyc_Core_Opt*_tmp247;struct Cyc_Absyn_Exp*_tmp248;
struct Cyc_Absyn_Exp*_tmp24A;enum Cyc_Absyn_Incrementor _tmp24B;struct Cyc_Absyn_Exp*
_tmp24D;struct Cyc_Absyn_Exp*_tmp24E;struct Cyc_Absyn_Exp*_tmp24F;struct Cyc_Absyn_Exp*
_tmp251;struct Cyc_Absyn_Exp*_tmp252;struct Cyc_Absyn_Exp*_tmp254;struct Cyc_Absyn_Exp*
_tmp255;struct Cyc_Absyn_Exp*_tmp257;struct Cyc_Absyn_Exp*_tmp258;struct Cyc_Absyn_Exp*
_tmp25A;struct Cyc_List_List*_tmp25B;struct Cyc_Absyn_Exp*_tmp25D;struct Cyc_List_List*
_tmp25E;struct Cyc_Absyn_Exp*_tmp260;struct Cyc_Absyn_Exp*_tmp262;struct Cyc_Absyn_Exp*
_tmp264;void*_tmp266;struct Cyc_Absyn_Exp*_tmp267;struct Cyc_Absyn_Exp*_tmp269;
struct Cyc_Absyn_Exp*_tmp26B;struct Cyc_Absyn_Exp*_tmp26C;void*_tmp26E;struct Cyc_Absyn_Exp*
_tmp270;void*_tmp272;struct Cyc_Absyn_Exp*_tmp274;struct _dyneither_ptr*_tmp275;
void*_tmp277;void*_tmp278;struct _dyneither_ptr*_tmp27A;void*_tmp27C;void*_tmp27D;
unsigned int _tmp27F;struct Cyc_List_List*_tmp281;void*_tmp282;struct Cyc_Absyn_Exp*
_tmp284;struct Cyc_Absyn_Exp*_tmp286;struct _dyneither_ptr*_tmp287;struct Cyc_Absyn_Exp*
_tmp289;struct _dyneither_ptr*_tmp28A;struct Cyc_Absyn_Exp*_tmp28C;struct Cyc_Absyn_Exp*
_tmp28D;struct Cyc_List_List*_tmp28F;struct _tuple7*_tmp291;struct Cyc_List_List*
_tmp292;struct Cyc_List_List*_tmp294;struct Cyc_Absyn_Vardecl*_tmp296;struct Cyc_Absyn_Exp*
_tmp297;struct Cyc_Absyn_Exp*_tmp298;struct _tuple0*_tmp29A;struct Cyc_List_List*
_tmp29B;struct Cyc_List_List*_tmp29C;struct Cyc_List_List*_tmp29E;struct Cyc_List_List*
_tmp2A0;struct Cyc_Absyn_Datatypefield*_tmp2A1;struct _tuple0*_tmp2A3;struct
_tuple0*_tmp2A5;struct Cyc_Absyn_MallocInfo _tmp2A7;int _tmp2A8;struct Cyc_Absyn_Exp*
_tmp2A9;void**_tmp2AA;struct Cyc_Absyn_Exp*_tmp2AB;struct Cyc_Absyn_Exp*_tmp2AD;
struct Cyc_Absyn_Exp*_tmp2AE;struct Cyc_Core_Opt*_tmp2B0;struct Cyc_List_List*
_tmp2B1;struct Cyc_Absyn_Stmt*_tmp2B3;_LL188: {struct Cyc_Absyn_Const_e_struct*
_tmp23C=(struct Cyc_Absyn_Const_e_struct*)_tmp23B;if(_tmp23C->tag != 0)goto _LL18A;
else{_tmp23D=_tmp23C->f1;}}_LL189: s=Cyc_Absynpp_cnst2doc(_tmp23D);goto _LL187;
_LL18A: {struct Cyc_Absyn_Var_e_struct*_tmp23E=(struct Cyc_Absyn_Var_e_struct*)
_tmp23B;if(_tmp23E->tag != 1)goto _LL18C;else{_tmp23F=_tmp23E->f1;}}_LL18B: _tmp241=
_tmp23F;goto _LL18D;_LL18C: {struct Cyc_Absyn_UnknownId_e_struct*_tmp240=(struct
Cyc_Absyn_UnknownId_e_struct*)_tmp23B;if(_tmp240->tag != 2)goto _LL18E;else{
_tmp241=_tmp240->f1;}}_LL18D: s=Cyc_Absynpp_qvar2doc(_tmp241);goto _LL187;_LL18E: {
struct Cyc_Absyn_Primop_e_struct*_tmp242=(struct Cyc_Absyn_Primop_e_struct*)
_tmp23B;if(_tmp242->tag != 3)goto _LL190;else{_tmp243=_tmp242->f1;_tmp244=_tmp242->f2;}}
_LL18F: s=Cyc_Absynpp_primapp2doc(myprec,_tmp243,_tmp244);goto _LL187;_LL190: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp245=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp23B;if(_tmp245->tag != 4)goto _LL192;else{_tmp246=_tmp245->f1;_tmp247=_tmp245->f2;
_tmp248=_tmp245->f3;}}_LL191:{const char*_tmp767;const char*_tmp766;struct Cyc_PP_Doc*
_tmp765[5];s=((_tmp765[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp248),((_tmp765[3]=
Cyc_PP_text(((_tmp766="= ",_tag_dyneither(_tmp766,sizeof(char),3)))),((_tmp765[2]=
_tmp247 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp247->v),((
_tmp765[1]=Cyc_PP_text(((_tmp767=" ",_tag_dyneither(_tmp767,sizeof(char),2)))),((
_tmp765[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp246),Cyc_PP_cat(_tag_dyneither(
_tmp765,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL187;_LL192: {struct Cyc_Absyn_Increment_e_struct*
_tmp249=(struct Cyc_Absyn_Increment_e_struct*)_tmp23B;if(_tmp249->tag != 5)goto
_LL194;else{_tmp24A=_tmp249->f1;_tmp24B=_tmp249->f2;}}_LL193: {struct Cyc_PP_Doc*
_tmp2B7=Cyc_Absynpp_exp2doc_prec(myprec,_tmp24A);switch(_tmp24B){case Cyc_Absyn_PreInc:
_LL1DC:{const char*_tmp76A;struct Cyc_PP_Doc*_tmp769[2];s=((_tmp769[1]=_tmp2B7,((
_tmp769[0]=Cyc_PP_text(((_tmp76A="++",_tag_dyneither(_tmp76A,sizeof(char),3)))),
Cyc_PP_cat(_tag_dyneither(_tmp769,sizeof(struct Cyc_PP_Doc*),2))))));}break;case
Cyc_Absyn_PreDec: _LL1DD:{const char*_tmp76D;struct Cyc_PP_Doc*_tmp76C[2];s=((
_tmp76C[1]=_tmp2B7,((_tmp76C[0]=Cyc_PP_text(((_tmp76D="--",_tag_dyneither(
_tmp76D,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp76C,sizeof(struct Cyc_PP_Doc*),
2))))));}break;case Cyc_Absyn_PostInc: _LL1DE:{const char*_tmp770;struct Cyc_PP_Doc*
_tmp76F[2];s=((_tmp76F[1]=Cyc_PP_text(((_tmp770="++",_tag_dyneither(_tmp770,
sizeof(char),3)))),((_tmp76F[0]=_tmp2B7,Cyc_PP_cat(_tag_dyneither(_tmp76F,
sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostDec: _LL1DF:{const char*
_tmp773;struct Cyc_PP_Doc*_tmp772[2];s=((_tmp772[1]=Cyc_PP_text(((_tmp773="--",
_tag_dyneither(_tmp773,sizeof(char),3)))),((_tmp772[0]=_tmp2B7,Cyc_PP_cat(
_tag_dyneither(_tmp772,sizeof(struct Cyc_PP_Doc*),2))))));}break;}goto _LL187;}
_LL194: {struct Cyc_Absyn_Conditional_e_struct*_tmp24C=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp23B;if(_tmp24C->tag != 6)goto _LL196;else{_tmp24D=_tmp24C->f1;_tmp24E=_tmp24C->f2;
_tmp24F=_tmp24C->f3;}}_LL195:{const char*_tmp778;const char*_tmp777;struct Cyc_PP_Doc*
_tmp776[5];s=((_tmp776[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp24F),((_tmp776[3]=
Cyc_PP_text(((_tmp777=" : ",_tag_dyneither(_tmp777,sizeof(char),4)))),((_tmp776[
2]=Cyc_Absynpp_exp2doc_prec(0,_tmp24E),((_tmp776[1]=Cyc_PP_text(((_tmp778=" ? ",
_tag_dyneither(_tmp778,sizeof(char),4)))),((_tmp776[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp24D),Cyc_PP_cat(_tag_dyneither(_tmp776,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL187;_LL196: {struct Cyc_Absyn_And_e_struct*_tmp250=(struct Cyc_Absyn_And_e_struct*)
_tmp23B;if(_tmp250->tag != 7)goto _LL198;else{_tmp251=_tmp250->f1;_tmp252=_tmp250->f2;}}
_LL197:{const char*_tmp77B;struct Cyc_PP_Doc*_tmp77A[3];s=((_tmp77A[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp252),((_tmp77A[1]=Cyc_PP_text(((_tmp77B=" && ",_tag_dyneither(_tmp77B,
sizeof(char),5)))),((_tmp77A[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp251),Cyc_PP_cat(
_tag_dyneither(_tmp77A,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL187;_LL198: {
struct Cyc_Absyn_Or_e_struct*_tmp253=(struct Cyc_Absyn_Or_e_struct*)_tmp23B;if(
_tmp253->tag != 8)goto _LL19A;else{_tmp254=_tmp253->f1;_tmp255=_tmp253->f2;}}
_LL199:{const char*_tmp77E;struct Cyc_PP_Doc*_tmp77D[3];s=((_tmp77D[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp255),((_tmp77D[1]=Cyc_PP_text(((_tmp77E=" || ",_tag_dyneither(_tmp77E,
sizeof(char),5)))),((_tmp77D[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp254),Cyc_PP_cat(
_tag_dyneither(_tmp77D,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL187;_LL19A: {
struct Cyc_Absyn_SeqExp_e_struct*_tmp256=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp23B;if(_tmp256->tag != 9)goto _LL19C;else{_tmp257=_tmp256->f1;_tmp258=_tmp256->f2;}}
_LL19B:{const char*_tmp785;const char*_tmp784;const char*_tmp783;struct Cyc_PP_Doc*
_tmp782[5];s=((_tmp782[4]=Cyc_PP_text(((_tmp783=")",_tag_dyneither(_tmp783,
sizeof(char),2)))),((_tmp782[3]=Cyc_Absynpp_exp2doc(_tmp258),((_tmp782[2]=Cyc_PP_text(((
_tmp784=", ",_tag_dyneither(_tmp784,sizeof(char),3)))),((_tmp782[1]=Cyc_Absynpp_exp2doc(
_tmp257),((_tmp782[0]=Cyc_PP_text(((_tmp785="(",_tag_dyneither(_tmp785,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp782,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL187;_LL19C: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp259=(struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp23B;if(_tmp259->tag != 10)goto _LL19E;else{_tmp25A=_tmp259->f1;_tmp25B=_tmp259->f2;}}
_LL19D:{const char*_tmp78A;const char*_tmp789;struct Cyc_PP_Doc*_tmp788[4];s=((
_tmp788[3]=Cyc_PP_text(((_tmp789=")",_tag_dyneither(_tmp789,sizeof(char),2)))),((
_tmp788[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp25B),((_tmp788[1]=Cyc_PP_text(((
_tmp78A="(",_tag_dyneither(_tmp78A,sizeof(char),2)))),((_tmp788[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp25A),Cyc_PP_cat(_tag_dyneither(_tmp788,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL187;_LL19E: {struct Cyc_Absyn_FnCall_e_struct*_tmp25C=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp23B;if(_tmp25C->tag != 11)goto _LL1A0;else{_tmp25D=_tmp25C->f1;_tmp25E=_tmp25C->f2;}}
_LL19F:{const char*_tmp78F;const char*_tmp78E;struct Cyc_PP_Doc*_tmp78D[4];s=((
_tmp78D[3]=Cyc_PP_text(((_tmp78E=")",_tag_dyneither(_tmp78E,sizeof(char),2)))),((
_tmp78D[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp25E),((_tmp78D[1]=Cyc_PP_text(((
_tmp78F="(",_tag_dyneither(_tmp78F,sizeof(char),2)))),((_tmp78D[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp25D),Cyc_PP_cat(_tag_dyneither(_tmp78D,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL187;_LL1A0: {struct Cyc_Absyn_Throw_e_struct*_tmp25F=(struct Cyc_Absyn_Throw_e_struct*)
_tmp23B;if(_tmp25F->tag != 12)goto _LL1A2;else{_tmp260=_tmp25F->f1;}}_LL1A1:{const
char*_tmp792;struct Cyc_PP_Doc*_tmp791[2];s=((_tmp791[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp260),((_tmp791[0]=Cyc_PP_text(((_tmp792="throw ",_tag_dyneither(
_tmp792,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp791,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LL187;_LL1A2: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp261=(
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp23B;if(_tmp261->tag != 13)goto _LL1A4;
else{_tmp262=_tmp261->f1;}}_LL1A3: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp262);
goto _LL187;_LL1A4: {struct Cyc_Absyn_Instantiate_e_struct*_tmp263=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp23B;if(_tmp263->tag != 14)goto _LL1A6;else{_tmp264=_tmp263->f1;}}_LL1A5: s=Cyc_Absynpp_exp2doc_prec(
inprec,_tmp264);goto _LL187;_LL1A6: {struct Cyc_Absyn_Cast_e_struct*_tmp265=(
struct Cyc_Absyn_Cast_e_struct*)_tmp23B;if(_tmp265->tag != 15)goto _LL1A8;else{
_tmp266=(void*)_tmp265->f1;_tmp267=_tmp265->f2;}}_LL1A7:{const char*_tmp797;const
char*_tmp796;struct Cyc_PP_Doc*_tmp795[4];s=((_tmp795[3]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp267),((_tmp795[2]=Cyc_PP_text(((_tmp796=")",_tag_dyneither(_tmp796,
sizeof(char),2)))),((_tmp795[1]=Cyc_Absynpp_typ2doc(_tmp266),((_tmp795[0]=Cyc_PP_text(((
_tmp797="(",_tag_dyneither(_tmp797,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp795,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL187;_LL1A8: {struct Cyc_Absyn_Address_e_struct*
_tmp268=(struct Cyc_Absyn_Address_e_struct*)_tmp23B;if(_tmp268->tag != 16)goto
_LL1AA;else{_tmp269=_tmp268->f1;}}_LL1A9:{const char*_tmp79A;struct Cyc_PP_Doc*
_tmp799[2];s=((_tmp799[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp269),((_tmp799[0]=
Cyc_PP_text(((_tmp79A="&",_tag_dyneither(_tmp79A,sizeof(char),2)))),Cyc_PP_cat(
_tag_dyneither(_tmp799,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL187;_LL1AA: {
struct Cyc_Absyn_New_e_struct*_tmp26A=(struct Cyc_Absyn_New_e_struct*)_tmp23B;if(
_tmp26A->tag != 17)goto _LL1AC;else{_tmp26B=_tmp26A->f1;_tmp26C=_tmp26A->f2;}}
_LL1AB: if(_tmp26B == 0){const char*_tmp79D;struct Cyc_PP_Doc*_tmp79C[2];s=((_tmp79C[
1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp26C),((_tmp79C[0]=Cyc_PP_text(((_tmp79D="new ",
_tag_dyneither(_tmp79D,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp79C,
sizeof(struct Cyc_PP_Doc*),2))))));}else{const char*_tmp7A2;const char*_tmp7A1;
struct Cyc_PP_Doc*_tmp7A0[4];s=((_tmp7A0[3]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmp26C),((_tmp7A0[2]=Cyc_PP_text(((_tmp7A1=") ",_tag_dyneither(_tmp7A1,sizeof(
char),3)))),((_tmp7A0[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp26B),((
_tmp7A0[0]=Cyc_PP_text(((_tmp7A2="rnew(",_tag_dyneither(_tmp7A2,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmp7A0,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto
_LL187;_LL1AC: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp26D=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp23B;if(_tmp26D->tag != 18)goto _LL1AE;else{_tmp26E=(void*)_tmp26D->f1;}}_LL1AD:{
const char*_tmp7A7;const char*_tmp7A6;struct Cyc_PP_Doc*_tmp7A5[3];s=((_tmp7A5[2]=
Cyc_PP_text(((_tmp7A6=")",_tag_dyneither(_tmp7A6,sizeof(char),2)))),((_tmp7A5[1]=
Cyc_Absynpp_typ2doc(_tmp26E),((_tmp7A5[0]=Cyc_PP_text(((_tmp7A7="sizeof(",
_tag_dyneither(_tmp7A7,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7A5,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL187;_LL1AE: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp26F=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp23B;if(_tmp26F->tag != 19)goto
_LL1B0;else{_tmp270=_tmp26F->f1;}}_LL1AF:{const char*_tmp7AC;const char*_tmp7AB;
struct Cyc_PP_Doc*_tmp7AA[3];s=((_tmp7AA[2]=Cyc_PP_text(((_tmp7AB=")",
_tag_dyneither(_tmp7AB,sizeof(char),2)))),((_tmp7AA[1]=Cyc_Absynpp_exp2doc(
_tmp270),((_tmp7AA[0]=Cyc_PP_text(((_tmp7AC="sizeof(",_tag_dyneither(_tmp7AC,
sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7AA,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL187;_LL1B0: {struct Cyc_Absyn_Valueof_e_struct*_tmp271=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp23B;if(_tmp271->tag != 40)goto _LL1B2;else{_tmp272=(void*)_tmp271->f1;}}_LL1B1:{
const char*_tmp7B1;const char*_tmp7B0;struct Cyc_PP_Doc*_tmp7AF[3];s=((_tmp7AF[2]=
Cyc_PP_text(((_tmp7B0=")",_tag_dyneither(_tmp7B0,sizeof(char),2)))),((_tmp7AF[1]=
Cyc_Absynpp_typ2doc(_tmp272),((_tmp7AF[0]=Cyc_PP_text(((_tmp7B1="valueof(",
_tag_dyneither(_tmp7B1,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7AF,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL187;_LL1B2: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp273=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp23B;if(_tmp273->tag != 39)goto
_LL1B4;else{_tmp274=_tmp273->f1;_tmp275=_tmp273->f2;}}_LL1B3:{const char*_tmp7B8;
const char*_tmp7B7;const char*_tmp7B6;struct Cyc_PP_Doc*_tmp7B5[5];s=((_tmp7B5[4]=
Cyc_PP_text(((_tmp7B6=")",_tag_dyneither(_tmp7B6,sizeof(char),2)))),((_tmp7B5[3]=
Cyc_PP_textptr(_tmp275),((_tmp7B5[2]=Cyc_PP_text(((_tmp7B7=".",_tag_dyneither(
_tmp7B7,sizeof(char),2)))),((_tmp7B5[1]=Cyc_Absynpp_exp2doc(_tmp274),((_tmp7B5[0]=
Cyc_PP_text(((_tmp7B8="tagcheck(",_tag_dyneither(_tmp7B8,sizeof(char),10)))),Cyc_PP_cat(
_tag_dyneither(_tmp7B5,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL187;
_LL1B4: {struct Cyc_Absyn_Offsetof_e_struct*_tmp276=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp23B;if(_tmp276->tag != 20)goto _LL1B6;else{_tmp277=(void*)_tmp276->f1;_tmp278=(
void*)_tmp276->f2;{struct Cyc_Absyn_StructField_struct*_tmp279=(struct Cyc_Absyn_StructField_struct*)
_tmp278;if(_tmp279->tag != 0)goto _LL1B6;else{_tmp27A=_tmp279->f1;}};}}_LL1B5:{
const char*_tmp7BF;const char*_tmp7BE;const char*_tmp7BD;struct Cyc_PP_Doc*_tmp7BC[5];
s=((_tmp7BC[4]=Cyc_PP_text(((_tmp7BD=")",_tag_dyneither(_tmp7BD,sizeof(char),2)))),((
_tmp7BC[3]=Cyc_PP_textptr(_tmp27A),((_tmp7BC[2]=Cyc_PP_text(((_tmp7BE=",",
_tag_dyneither(_tmp7BE,sizeof(char),2)))),((_tmp7BC[1]=Cyc_Absynpp_typ2doc(
_tmp277),((_tmp7BC[0]=Cyc_PP_text(((_tmp7BF="offsetof(",_tag_dyneither(_tmp7BF,
sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7BC,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LL187;_LL1B6: {struct Cyc_Absyn_Offsetof_e_struct*_tmp27B=(
struct Cyc_Absyn_Offsetof_e_struct*)_tmp23B;if(_tmp27B->tag != 20)goto _LL1B8;else{
_tmp27C=(void*)_tmp27B->f1;_tmp27D=(void*)_tmp27B->f2;{struct Cyc_Absyn_TupleIndex_struct*
_tmp27E=(struct Cyc_Absyn_TupleIndex_struct*)_tmp27D;if(_tmp27E->tag != 1)goto
_LL1B8;else{_tmp27F=_tmp27E->f1;}};}}_LL1B7:{const char*_tmp7CD;const char*_tmp7CC;
struct Cyc_Int_pa_struct _tmp7CB;void*_tmp7CA[1];const char*_tmp7C9;const char*
_tmp7C8;struct Cyc_PP_Doc*_tmp7C7[5];s=((_tmp7C7[4]=Cyc_PP_text(((_tmp7C8=")",
_tag_dyneither(_tmp7C8,sizeof(char),2)))),((_tmp7C7[3]=Cyc_PP_text((struct
_dyneither_ptr)((_tmp7CB.tag=1,((_tmp7CB.f1=(unsigned long)((int)_tmp27F),((
_tmp7CA[0]=& _tmp7CB,Cyc_aprintf(((_tmp7C9="%d",_tag_dyneither(_tmp7C9,sizeof(
char),3))),_tag_dyneither(_tmp7CA,sizeof(void*),1))))))))),((_tmp7C7[2]=Cyc_PP_text(((
_tmp7CC=",",_tag_dyneither(_tmp7CC,sizeof(char),2)))),((_tmp7C7[1]=Cyc_Absynpp_typ2doc(
_tmp27C),((_tmp7C7[0]=Cyc_PP_text(((_tmp7CD="offsetof(",_tag_dyneither(_tmp7CD,
sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7C7,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LL187;_LL1B8: {struct Cyc_Absyn_Gentyp_e_struct*_tmp280=(
struct Cyc_Absyn_Gentyp_e_struct*)_tmp23B;if(_tmp280->tag != 21)goto _LL1BA;else{
_tmp281=_tmp280->f1;_tmp282=(void*)_tmp280->f2;}}_LL1B9:{const char*_tmp7D2;const
char*_tmp7D1;struct Cyc_PP_Doc*_tmp7D0[4];s=((_tmp7D0[3]=Cyc_PP_text(((_tmp7D1=")",
_tag_dyneither(_tmp7D1,sizeof(char),2)))),((_tmp7D0[2]=Cyc_Absynpp_typ2doc(
_tmp282),((_tmp7D0[1]=Cyc_Absynpp_tvars2doc(_tmp281),((_tmp7D0[0]=Cyc_PP_text(((
_tmp7D2="__gen(",_tag_dyneither(_tmp7D2,sizeof(char),7)))),Cyc_PP_cat(
_tag_dyneither(_tmp7D0,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL187;_LL1BA: {
struct Cyc_Absyn_Deref_e_struct*_tmp283=(struct Cyc_Absyn_Deref_e_struct*)_tmp23B;
if(_tmp283->tag != 22)goto _LL1BC;else{_tmp284=_tmp283->f1;}}_LL1BB:{const char*
_tmp7D5;struct Cyc_PP_Doc*_tmp7D4[2];s=((_tmp7D4[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp284),((_tmp7D4[0]=Cyc_PP_text(((_tmp7D5="*",_tag_dyneither(_tmp7D5,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7D4,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL187;_LL1BC: {struct Cyc_Absyn_AggrMember_e_struct*_tmp285=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp23B;if(_tmp285->tag != 23)goto _LL1BE;else{_tmp286=_tmp285->f1;_tmp287=_tmp285->f2;}}
_LL1BD:{const char*_tmp7D8;struct Cyc_PP_Doc*_tmp7D7[3];s=((_tmp7D7[2]=Cyc_PP_textptr(
_tmp287),((_tmp7D7[1]=Cyc_PP_text(((_tmp7D8=".",_tag_dyneither(_tmp7D8,sizeof(
char),2)))),((_tmp7D7[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp286),Cyc_PP_cat(
_tag_dyneither(_tmp7D7,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL187;_LL1BE: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp288=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp23B;if(_tmp288->tag != 24)goto _LL1C0;else{_tmp289=_tmp288->f1;_tmp28A=_tmp288->f2;}}
_LL1BF:{const char*_tmp7DB;struct Cyc_PP_Doc*_tmp7DA[3];s=((_tmp7DA[2]=Cyc_PP_textptr(
_tmp28A),((_tmp7DA[1]=Cyc_PP_text(((_tmp7DB="->",_tag_dyneither(_tmp7DB,sizeof(
char),3)))),((_tmp7DA[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp289),Cyc_PP_cat(
_tag_dyneither(_tmp7DA,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL187;_LL1C0: {
struct Cyc_Absyn_Subscript_e_struct*_tmp28B=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp23B;if(_tmp28B->tag != 25)goto _LL1C2;else{_tmp28C=_tmp28B->f1;_tmp28D=_tmp28B->f2;}}
_LL1C1:{const char*_tmp7E0;const char*_tmp7DF;struct Cyc_PP_Doc*_tmp7DE[4];s=((
_tmp7DE[3]=Cyc_PP_text(((_tmp7DF="]",_tag_dyneither(_tmp7DF,sizeof(char),2)))),((
_tmp7DE[2]=Cyc_Absynpp_exp2doc(_tmp28D),((_tmp7DE[1]=Cyc_PP_text(((_tmp7E0="[",
_tag_dyneither(_tmp7E0,sizeof(char),2)))),((_tmp7DE[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp28C),Cyc_PP_cat(_tag_dyneither(_tmp7DE,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL187;_LL1C2: {struct Cyc_Absyn_Tuple_e_struct*_tmp28E=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp23B;if(_tmp28E->tag != 26)goto _LL1C4;else{_tmp28F=_tmp28E->f1;}}_LL1C3:{const
char*_tmp7E5;const char*_tmp7E4;struct Cyc_PP_Doc*_tmp7E3[4];s=((_tmp7E3[3]=Cyc_PP_text(((
_tmp7E4=")",_tag_dyneither(_tmp7E4,sizeof(char),2)))),((_tmp7E3[2]=Cyc_Absynpp_exps2doc_prec(
20,_tmp28F),((_tmp7E3[1]=Cyc_PP_text(((_tmp7E5="(",_tag_dyneither(_tmp7E5,
sizeof(char),2)))),((_tmp7E3[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(
_tmp7E3,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL187;_LL1C4: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp290=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp23B;if(_tmp290->tag != 27)goto
_LL1C6;else{_tmp291=_tmp290->f1;_tmp292=_tmp290->f2;}}_LL1C5:{const char*_tmp7EC;
const char*_tmp7EB;const char*_tmp7EA;struct Cyc_PP_Doc*_tmp7E9[4];s=((_tmp7E9[3]=
Cyc_Absynpp_group_braces(((_tmp7EA=",",_tag_dyneither(_tmp7EA,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp292)),((_tmp7E9[2]=Cyc_PP_text(((_tmp7EB=")",
_tag_dyneither(_tmp7EB,sizeof(char),2)))),((_tmp7E9[1]=Cyc_Absynpp_typ2doc((*
_tmp291).f3),((_tmp7E9[0]=Cyc_PP_text(((_tmp7EC="(",_tag_dyneither(_tmp7EC,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7E9,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL187;_LL1C6: {struct Cyc_Absyn_Array_e_struct*_tmp293=(struct Cyc_Absyn_Array_e_struct*)
_tmp23B;if(_tmp293->tag != 28)goto _LL1C8;else{_tmp294=_tmp293->f1;}}_LL1C7:{const
char*_tmp7ED;s=Cyc_Absynpp_group_braces(((_tmp7ED=",",_tag_dyneither(_tmp7ED,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp294));}goto _LL187;
_LL1C8: {struct Cyc_Absyn_Comprehension_e_struct*_tmp295=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp23B;if(_tmp295->tag != 29)goto _LL1CA;else{_tmp296=_tmp295->f1;_tmp297=_tmp295->f2;
_tmp298=_tmp295->f3;}}_LL1C9:{const char*_tmp7F4;const char*_tmp7F3;const char*
_tmp7F2;struct Cyc_PP_Doc*_tmp7F1[8];s=((_tmp7F1[7]=Cyc_Absynpp_rb(),((_tmp7F1[6]=
Cyc_Absynpp_exp2doc(_tmp298),((_tmp7F1[5]=Cyc_PP_text(((_tmp7F2=" : ",
_tag_dyneither(_tmp7F2,sizeof(char),4)))),((_tmp7F1[4]=Cyc_Absynpp_exp2doc(
_tmp297),((_tmp7F1[3]=Cyc_PP_text(((_tmp7F3=" < ",_tag_dyneither(_tmp7F3,sizeof(
char),4)))),((_tmp7F1[2]=Cyc_PP_text(*(*_tmp296->name).f2),((_tmp7F1[1]=Cyc_PP_text(((
_tmp7F4="for ",_tag_dyneither(_tmp7F4,sizeof(char),5)))),((_tmp7F1[0]=Cyc_Absynpp_lb(),
Cyc_PP_cat(_tag_dyneither(_tmp7F1,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL187;_LL1CA: {struct Cyc_Absyn_Aggregate_e_struct*_tmp299=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp23B;if(_tmp299->tag != 30)goto _LL1CC;else{_tmp29A=_tmp299->f1;_tmp29B=_tmp299->f2;
_tmp29C=_tmp299->f3;}}_LL1CB: {struct Cyc_List_List*_tmp30D=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp29C);{const char*_tmp7F9;struct Cyc_List_List*_tmp7F8;struct Cyc_PP_Doc*_tmp7F7[
2];s=((_tmp7F7[1]=Cyc_Absynpp_group_braces(((_tmp7F9=",",_tag_dyneither(_tmp7F9,
sizeof(char),2))),_tmp29B != 0?(_tmp7F8=_cycalloc(sizeof(*_tmp7F8)),((_tmp7F8->hd=
Cyc_Absynpp_tps2doc(_tmp29B),((_tmp7F8->tl=_tmp30D,_tmp7F8))))): _tmp30D),((
_tmp7F7[0]=Cyc_Absynpp_qvar2doc(_tmp29A),Cyc_PP_cat(_tag_dyneither(_tmp7F7,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL187;}_LL1CC: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp29D=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp23B;if(_tmp29D->tag != 31)goto
_LL1CE;else{_tmp29E=_tmp29D->f2;}}_LL1CD:{const char*_tmp7FA;s=Cyc_Absynpp_group_braces(((
_tmp7FA=",",_tag_dyneither(_tmp7FA,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp29E));}goto _LL187;_LL1CE: {struct Cyc_Absyn_Datatype_e_struct*_tmp29F=(struct
Cyc_Absyn_Datatype_e_struct*)_tmp23B;if(_tmp29F->tag != 32)goto _LL1D0;else{
_tmp2A0=_tmp29F->f1;_tmp2A1=_tmp29F->f3;}}_LL1CF: if(_tmp2A0 == 0)s=Cyc_Absynpp_qvar2doc(
_tmp2A1->name);else{const char*_tmp801;const char*_tmp800;const char*_tmp7FF;struct
Cyc_PP_Doc*_tmp7FE[2];s=((_tmp7FE[1]=Cyc_PP_egroup(((_tmp801="(",_tag_dyneither(
_tmp801,sizeof(char),2))),((_tmp800=")",_tag_dyneither(_tmp800,sizeof(char),2))),((
_tmp7FF=",",_tag_dyneither(_tmp7FF,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_exp2doc,_tmp2A0)),((_tmp7FE[0]=Cyc_Absynpp_qvar2doc(_tmp2A1->name),
Cyc_PP_cat(_tag_dyneither(_tmp7FE,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL187;
_LL1D0: {struct Cyc_Absyn_Enum_e_struct*_tmp2A2=(struct Cyc_Absyn_Enum_e_struct*)
_tmp23B;if(_tmp2A2->tag != 33)goto _LL1D2;else{_tmp2A3=_tmp2A2->f1;}}_LL1D1: s=Cyc_Absynpp_qvar2doc(
_tmp2A3);goto _LL187;_LL1D2: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp2A4=(struct
Cyc_Absyn_AnonEnum_e_struct*)_tmp23B;if(_tmp2A4->tag != 34)goto _LL1D4;else{
_tmp2A5=_tmp2A4->f1;}}_LL1D3: s=Cyc_Absynpp_qvar2doc(_tmp2A5);goto _LL187;_LL1D4: {
struct Cyc_Absyn_Malloc_e_struct*_tmp2A6=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp23B;if(_tmp2A6->tag != 35)goto _LL1D6;else{_tmp2A7=_tmp2A6->f1;_tmp2A8=_tmp2A7.is_calloc;
_tmp2A9=_tmp2A7.rgn;_tmp2AA=_tmp2A7.elt_type;_tmp2AB=_tmp2A7.num_elts;}}_LL1D5:
if(_tmp2A8){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(
_tmp2AA)),0);if(_tmp2A9 == 0){const char*_tmp808;const char*_tmp807;const char*
_tmp806;struct Cyc_PP_Doc*_tmp805[5];s=((_tmp805[4]=Cyc_PP_text(((_tmp806=")",
_tag_dyneither(_tmp806,sizeof(char),2)))),((_tmp805[3]=Cyc_Absynpp_exp2doc(st),((
_tmp805[2]=Cyc_PP_text(((_tmp807=",",_tag_dyneither(_tmp807,sizeof(char),2)))),((
_tmp805[1]=Cyc_Absynpp_exp2doc(_tmp2AB),((_tmp805[0]=Cyc_PP_text(((_tmp808="calloc(",
_tag_dyneither(_tmp808,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp805,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{const char*_tmp811;const char*
_tmp810;const char*_tmp80F;const char*_tmp80E;struct Cyc_PP_Doc*_tmp80D[7];s=((
_tmp80D[6]=Cyc_PP_text(((_tmp80E=")",_tag_dyneither(_tmp80E,sizeof(char),2)))),((
_tmp80D[5]=Cyc_Absynpp_exp2doc(st),((_tmp80D[4]=Cyc_PP_text(((_tmp80F=",",
_tag_dyneither(_tmp80F,sizeof(char),2)))),((_tmp80D[3]=Cyc_Absynpp_exp2doc(
_tmp2AB),((_tmp80D[2]=Cyc_PP_text(((_tmp810=",",_tag_dyneither(_tmp810,sizeof(
char),2)))),((_tmp80D[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp2A9),((
_tmp80D[0]=Cyc_PP_text(((_tmp811="rcalloc(",_tag_dyneither(_tmp811,sizeof(char),
9)))),Cyc_PP_cat(_tag_dyneither(_tmp80D,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}
else{struct Cyc_Absyn_Exp*new_e;if(_tmp2AA == 0)new_e=_tmp2AB;else{new_e=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(*_tmp2AA,0),_tmp2AB,0);}if(_tmp2A9 == 0){const char*
_tmp816;const char*_tmp815;struct Cyc_PP_Doc*_tmp814[3];s=((_tmp814[2]=Cyc_PP_text(((
_tmp815=")",_tag_dyneither(_tmp815,sizeof(char),2)))),((_tmp814[1]=Cyc_Absynpp_exp2doc(
new_e),((_tmp814[0]=Cyc_PP_text(((_tmp816="malloc(",_tag_dyneither(_tmp816,
sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp814,sizeof(struct Cyc_PP_Doc*),3))))))));}
else{const char*_tmp81D;const char*_tmp81C;const char*_tmp81B;struct Cyc_PP_Doc*
_tmp81A[5];s=((_tmp81A[4]=Cyc_PP_text(((_tmp81B=")",_tag_dyneither(_tmp81B,
sizeof(char),2)))),((_tmp81A[3]=Cyc_Absynpp_exp2doc(new_e),((_tmp81A[2]=Cyc_PP_text(((
_tmp81C=",",_tag_dyneither(_tmp81C,sizeof(char),2)))),((_tmp81A[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp2A9),((_tmp81A[0]=Cyc_PP_text(((_tmp81D="rmalloc(",
_tag_dyneither(_tmp81D,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp81A,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}}goto _LL187;_LL1D6: {struct Cyc_Absyn_Swap_e_struct*
_tmp2AC=(struct Cyc_Absyn_Swap_e_struct*)_tmp23B;if(_tmp2AC->tag != 36)goto _LL1D8;
else{_tmp2AD=_tmp2AC->f1;_tmp2AE=_tmp2AC->f2;}}_LL1D7:{const char*_tmp824;const
char*_tmp823;const char*_tmp822;struct Cyc_PP_Doc*_tmp821[5];s=((_tmp821[4]=Cyc_PP_text(((
_tmp822=")",_tag_dyneither(_tmp822,sizeof(char),2)))),((_tmp821[3]=Cyc_Absynpp_exp2doc(
_tmp2AE),((_tmp821[2]=Cyc_PP_text(((_tmp823=",",_tag_dyneither(_tmp823,sizeof(
char),2)))),((_tmp821[1]=Cyc_Absynpp_exp2doc(_tmp2AD),((_tmp821[0]=Cyc_PP_text(((
_tmp824="cycswap(",_tag_dyneither(_tmp824,sizeof(char),9)))),Cyc_PP_cat(
_tag_dyneither(_tmp821,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL187;
_LL1D8: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp2AF=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp23B;if(_tmp2AF->tag != 37)goto _LL1DA;else{_tmp2B0=_tmp2AF->f1;_tmp2B1=_tmp2AF->f2;}}
_LL1D9:{const char*_tmp825;s=Cyc_Absynpp_group_braces(((_tmp825=",",
_tag_dyneither(_tmp825,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp2B1));}goto _LL187;_LL1DA: {struct Cyc_Absyn_StmtExp_e_struct*_tmp2B2=(struct
Cyc_Absyn_StmtExp_e_struct*)_tmp23B;if(_tmp2B2->tag != 38)goto _LL187;else{_tmp2B3=
_tmp2B2->f1;}}_LL1DB:{const char*_tmp82A;const char*_tmp829;struct Cyc_PP_Doc*
_tmp828[7];s=((_tmp828[6]=Cyc_PP_text(((_tmp829=")",_tag_dyneither(_tmp829,
sizeof(char),2)))),((_tmp828[5]=Cyc_Absynpp_rb(),((_tmp828[4]=Cyc_PP_blank_doc(),((
_tmp828[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2B3,1)),((_tmp828[2]=Cyc_PP_blank_doc(),((
_tmp828[1]=Cyc_Absynpp_lb(),((_tmp828[0]=Cyc_PP_text(((_tmp82A="(",
_tag_dyneither(_tmp82A,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp828,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}goto _LL187;_LL187:;}if(inprec >= 
myprec){const char*_tmp82F;const char*_tmp82E;struct Cyc_PP_Doc*_tmp82D[3];s=((
_tmp82D[2]=Cyc_PP_text(((_tmp82E=")",_tag_dyneither(_tmp82E,sizeof(char),2)))),((
_tmp82D[1]=s,((_tmp82D[0]=Cyc_PP_text(((_tmp82F="(",_tag_dyneither(_tmp82F,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp82D,sizeof(struct Cyc_PP_Doc*),3))))))));}
return s;}struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d);struct Cyc_PP_Doc*
Cyc_Absynpp_designator2doc(void*d){void*_tmp331=d;struct Cyc_Absyn_Exp*_tmp333;
struct _dyneither_ptr*_tmp335;_LL1E2: {struct Cyc_Absyn_ArrayElement_struct*
_tmp332=(struct Cyc_Absyn_ArrayElement_struct*)_tmp331;if(_tmp332->tag != 0)goto
_LL1E4;else{_tmp333=_tmp332->f1;}}_LL1E3: {const char*_tmp834;const char*_tmp833;
struct Cyc_PP_Doc*_tmp832[3];return(_tmp832[2]=Cyc_PP_text(((_tmp833="]",
_tag_dyneither(_tmp833,sizeof(char),2)))),((_tmp832[1]=Cyc_Absynpp_exp2doc(
_tmp333),((_tmp832[0]=Cyc_PP_text(((_tmp834=".[",_tag_dyneither(_tmp834,sizeof(
char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp832,sizeof(struct Cyc_PP_Doc*),3)))))));}
_LL1E4: {struct Cyc_Absyn_FieldName_struct*_tmp334=(struct Cyc_Absyn_FieldName_struct*)
_tmp331;if(_tmp334->tag != 1)goto _LL1E1;else{_tmp335=_tmp334->f1;}}_LL1E5: {const
char*_tmp837;struct Cyc_PP_Doc*_tmp836[2];return(_tmp836[1]=Cyc_PP_textptr(
_tmp335),((_tmp836[0]=Cyc_PP_text(((_tmp837=".",_tag_dyneither(_tmp837,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp836,sizeof(struct Cyc_PP_Doc*),2)))));}
_LL1E1:;}struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de);struct Cyc_PP_Doc*
Cyc_Absynpp_de2doc(struct _tuple14*de){if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*
de).f2);else{const char*_tmp83E;const char*_tmp83D;const char*_tmp83C;struct Cyc_PP_Doc*
_tmp83B[2];return(_tmp83B[1]=Cyc_Absynpp_exp2doc((*de).f2),((_tmp83B[0]=Cyc_PP_egroup(((
_tmp83E="",_tag_dyneither(_tmp83E,sizeof(char),1))),((_tmp83D="=",_tag_dyneither(
_tmp83D,sizeof(char),2))),((_tmp83C="=",_tag_dyneither(_tmp83C,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*de).f1)),Cyc_PP_cat(_tag_dyneither(_tmp83B,sizeof(
struct Cyc_PP_Doc*),2)))));}}struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,
struct Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,
struct Cyc_List_List*es){const char*_tmp841;const char*_tmp840;const char*_tmp83F;
return Cyc_PP_group(((_tmp83F="",_tag_dyneither(_tmp83F,sizeof(char),1))),((
_tmp840="",_tag_dyneither(_tmp840,sizeof(char),1))),((_tmp841=",",_tag_dyneither(
_tmp841,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,
struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,
inprec,es));}struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c);struct
Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){union Cyc_Absyn_Cnst _tmp342=
c;struct _tuple3 _tmp343;enum Cyc_Absyn_Sign _tmp344;char _tmp345;struct _tuple4
_tmp346;enum Cyc_Absyn_Sign _tmp347;short _tmp348;struct _tuple5 _tmp349;enum Cyc_Absyn_Sign
_tmp34A;int _tmp34B;struct _tuple5 _tmp34C;enum Cyc_Absyn_Sign _tmp34D;int _tmp34E;
struct _tuple5 _tmp34F;enum Cyc_Absyn_Sign _tmp350;int _tmp351;struct _tuple6 _tmp352;
enum Cyc_Absyn_Sign _tmp353;long long _tmp354;struct _dyneither_ptr _tmp355;int
_tmp356;struct _dyneither_ptr _tmp357;_LL1E7: if((_tmp342.Char_c).tag != 2)goto
_LL1E9;_tmp343=(struct _tuple3)(_tmp342.Char_c).val;_tmp344=_tmp343.f1;_tmp345=
_tmp343.f2;_LL1E8: {const char*_tmp845;void*_tmp844[1];struct Cyc_String_pa_struct
_tmp843;return Cyc_PP_text((struct _dyneither_ptr)((_tmp843.tag=0,((_tmp843.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp345)),((
_tmp844[0]=& _tmp843,Cyc_aprintf(((_tmp845="'%s'",_tag_dyneither(_tmp845,sizeof(
char),5))),_tag_dyneither(_tmp844,sizeof(void*),1)))))))));}_LL1E9: if((_tmp342.Short_c).tag
!= 3)goto _LL1EB;_tmp346=(struct _tuple4)(_tmp342.Short_c).val;_tmp347=_tmp346.f1;
_tmp348=_tmp346.f2;_LL1EA: {const char*_tmp849;void*_tmp848[1];struct Cyc_Int_pa_struct
_tmp847;return Cyc_PP_text((struct _dyneither_ptr)((_tmp847.tag=1,((_tmp847.f1=(
unsigned long)((int)_tmp348),((_tmp848[0]=& _tmp847,Cyc_aprintf(((_tmp849="%d",
_tag_dyneither(_tmp849,sizeof(char),3))),_tag_dyneither(_tmp848,sizeof(void*),1)))))))));}
_LL1EB: if((_tmp342.Int_c).tag != 4)goto _LL1ED;_tmp349=(struct _tuple5)(_tmp342.Int_c).val;
_tmp34A=_tmp349.f1;if(_tmp34A != Cyc_Absyn_None)goto _LL1ED;_tmp34B=_tmp349.f2;
_LL1EC: _tmp34E=_tmp34B;goto _LL1EE;_LL1ED: if((_tmp342.Int_c).tag != 4)goto _LL1EF;
_tmp34C=(struct _tuple5)(_tmp342.Int_c).val;_tmp34D=_tmp34C.f1;if(_tmp34D != Cyc_Absyn_Signed)
goto _LL1EF;_tmp34E=_tmp34C.f2;_LL1EE: {const char*_tmp84D;void*_tmp84C[1];struct
Cyc_Int_pa_struct _tmp84B;return Cyc_PP_text((struct _dyneither_ptr)((_tmp84B.tag=1,((
_tmp84B.f1=(unsigned long)_tmp34E,((_tmp84C[0]=& _tmp84B,Cyc_aprintf(((_tmp84D="%d",
_tag_dyneither(_tmp84D,sizeof(char),3))),_tag_dyneither(_tmp84C,sizeof(void*),1)))))))));}
_LL1EF: if((_tmp342.Int_c).tag != 4)goto _LL1F1;_tmp34F=(struct _tuple5)(_tmp342.Int_c).val;
_tmp350=_tmp34F.f1;if(_tmp350 != Cyc_Absyn_Unsigned)goto _LL1F1;_tmp351=_tmp34F.f2;
_LL1F0: {const char*_tmp851;void*_tmp850[1];struct Cyc_Int_pa_struct _tmp84F;return
Cyc_PP_text((struct _dyneither_ptr)((_tmp84F.tag=1,((_tmp84F.f1=(unsigned int)
_tmp351,((_tmp850[0]=& _tmp84F,Cyc_aprintf(((_tmp851="%u",_tag_dyneither(_tmp851,
sizeof(char),3))),_tag_dyneither(_tmp850,sizeof(void*),1)))))))));}_LL1F1: if((
_tmp342.LongLong_c).tag != 5)goto _LL1F3;_tmp352=(struct _tuple6)(_tmp342.LongLong_c).val;
_tmp353=_tmp352.f1;_tmp354=_tmp352.f2;_LL1F2: {const char*_tmp852;return Cyc_PP_text(((
_tmp852="<<FIX LONG LONG CONSTANT>>",_tag_dyneither(_tmp852,sizeof(char),27))));}
_LL1F3: if((_tmp342.Float_c).tag != 6)goto _LL1F5;_tmp355=(struct _dyneither_ptr)(
_tmp342.Float_c).val;_LL1F4: return Cyc_PP_text(_tmp355);_LL1F5: if((_tmp342.Null_c).tag
!= 1)goto _LL1F7;_tmp356=(int)(_tmp342.Null_c).val;_LL1F6: {const char*_tmp853;
return Cyc_PP_text(((_tmp853="NULL",_tag_dyneither(_tmp853,sizeof(char),5))));}
_LL1F7: if((_tmp342.String_c).tag != 7)goto _LL1E6;_tmp357=(struct _dyneither_ptr)(
_tmp342.String_c).val;_LL1F8: {const char*_tmp858;const char*_tmp857;struct Cyc_PP_Doc*
_tmp856[3];return(_tmp856[2]=Cyc_PP_text(((_tmp857="\"",_tag_dyneither(_tmp857,
sizeof(char),2)))),((_tmp856[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp357)),((
_tmp856[0]=Cyc_PP_text(((_tmp858="\"",_tag_dyneither(_tmp858,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp856,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL1E6:;}
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct
Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop
p,struct Cyc_List_List*es){struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);if(p == (
enum Cyc_Absyn_Primop)Cyc_Absyn_Numelts){if(es == 0  || es->tl != 0){struct Cyc_Core_Failure_struct
_tmp865;const char*_tmp864;void*_tmp863[1];struct Cyc_String_pa_struct _tmp862;
struct Cyc_Core_Failure_struct*_tmp861;(int)_throw((void*)((_tmp861=_cycalloc(
sizeof(*_tmp861)),((_tmp861[0]=((_tmp865.tag=Cyc_Core_Failure,((_tmp865.f1=(
struct _dyneither_ptr)((_tmp862.tag=0,((_tmp862.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp863[0]=& _tmp862,Cyc_aprintf(((
_tmp864="Absynpp::primapp2doc Numelts: %s with bad args",_tag_dyneither(_tmp864,
sizeof(char),47))),_tag_dyneither(_tmp863,sizeof(void*),1)))))))),_tmp865)))),
_tmp861)))));}{const char*_tmp86A;const char*_tmp869;struct Cyc_PP_Doc*_tmp868[3];
return(_tmp868[2]=Cyc_PP_text(((_tmp869=")",_tag_dyneither(_tmp869,sizeof(char),
2)))),((_tmp868[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd),((_tmp868[0]=
Cyc_PP_text(((_tmp86A="numelts(",_tag_dyneither(_tmp86A,sizeof(char),9)))),Cyc_PP_cat(
_tag_dyneither(_tmp868,sizeof(struct Cyc_PP_Doc*),3)))))));};}else{struct Cyc_List_List*
ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0){struct Cyc_Core_Failure_struct _tmp877;const char*_tmp876;void*_tmp875[1];
struct Cyc_String_pa_struct _tmp874;struct Cyc_Core_Failure_struct*_tmp873;(int)
_throw((void*)((_tmp873=_cycalloc(sizeof(*_tmp873)),((_tmp873[0]=((_tmp877.tag=
Cyc_Core_Failure,((_tmp877.f1=(struct _dyneither_ptr)((_tmp874.tag=0,((_tmp874.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((
_tmp875[0]=& _tmp874,Cyc_aprintf(((_tmp876="Absynpp::primapp2doc: %s with no args",
_tag_dyneither(_tmp876,sizeof(char),38))),_tag_dyneither(_tmp875,sizeof(void*),1)))))))),
_tmp877)))),_tmp873)))));}else{if(ds->tl == 0){const char*_tmp87A;struct Cyc_PP_Doc*
_tmp879[3];return(_tmp879[2]=(struct Cyc_PP_Doc*)ds->hd,((_tmp879[1]=Cyc_PP_text(((
_tmp87A=" ",_tag_dyneither(_tmp87A,sizeof(char),2)))),((_tmp879[0]=ps,Cyc_PP_cat(
_tag_dyneither(_tmp879,sizeof(struct Cyc_PP_Doc*),3)))))));}else{if(((struct Cyc_List_List*)
_check_null(ds->tl))->tl != 0){struct Cyc_Core_Failure_struct _tmp887;const char*
_tmp886;void*_tmp885[1];struct Cyc_String_pa_struct _tmp884;struct Cyc_Core_Failure_struct*
_tmp883;(int)_throw((void*)((_tmp883=_cycalloc(sizeof(*_tmp883)),((_tmp883[0]=((
_tmp887.tag=Cyc_Core_Failure,((_tmp887.f1=(struct _dyneither_ptr)((_tmp884.tag=0,((
_tmp884.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,
72)),((_tmp885[0]=& _tmp884,Cyc_aprintf(((_tmp886="Absynpp::primapp2doc: %s with more than 2 args",
_tag_dyneither(_tmp886,sizeof(char),47))),_tag_dyneither(_tmp885,sizeof(void*),1)))))))),
_tmp887)))),_tmp883)))));}else{const char*_tmp88C;const char*_tmp88B;struct Cyc_PP_Doc*
_tmp88A[5];return(_tmp88A[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)
_check_null(ds->tl))->hd,((_tmp88A[3]=Cyc_PP_text(((_tmp88B=" ",_tag_dyneither(
_tmp88B,sizeof(char),2)))),((_tmp88A[2]=ps,((_tmp88A[1]=Cyc_PP_text(((_tmp88C=" ",
_tag_dyneither(_tmp88C,sizeof(char),2)))),((_tmp88A[0]=(struct Cyc_PP_Doc*)ds->hd,
Cyc_PP_cat(_tag_dyneither(_tmp88A,sizeof(struct Cyc_PP_Doc*),5)))))))))));}}}}}
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p);struct
_dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){switch(p){case Cyc_Absyn_Plus:
_LL1F9: {const char*_tmp88D;return(_tmp88D="+",_tag_dyneither(_tmp88D,sizeof(char),
2));}case Cyc_Absyn_Times: _LL1FA: {const char*_tmp88E;return(_tmp88E="*",
_tag_dyneither(_tmp88E,sizeof(char),2));}case Cyc_Absyn_Minus: _LL1FB: {const char*
_tmp88F;return(_tmp88F="-",_tag_dyneither(_tmp88F,sizeof(char),2));}case Cyc_Absyn_Div:
_LL1FC: {const char*_tmp890;return(_tmp890="/",_tag_dyneither(_tmp890,sizeof(char),
2));}case Cyc_Absyn_Mod: _LL1FD: {const char*_tmp892;const char*_tmp891;return Cyc_Absynpp_print_for_cycdoc?(
_tmp892="\\%",_tag_dyneither(_tmp892,sizeof(char),3)):((_tmp891="%",
_tag_dyneither(_tmp891,sizeof(char),2)));}case Cyc_Absyn_Eq: _LL1FE: {const char*
_tmp893;return(_tmp893="==",_tag_dyneither(_tmp893,sizeof(char),3));}case Cyc_Absyn_Neq:
_LL1FF: {const char*_tmp894;return(_tmp894="!=",_tag_dyneither(_tmp894,sizeof(
char),3));}case Cyc_Absyn_Gt: _LL200: {const char*_tmp895;return(_tmp895=">",
_tag_dyneither(_tmp895,sizeof(char),2));}case Cyc_Absyn_Lt: _LL201: {const char*
_tmp896;return(_tmp896="<",_tag_dyneither(_tmp896,sizeof(char),2));}case Cyc_Absyn_Gte:
_LL202: {const char*_tmp897;return(_tmp897=">=",_tag_dyneither(_tmp897,sizeof(
char),3));}case Cyc_Absyn_Lte: _LL203: {const char*_tmp898;return(_tmp898="<=",
_tag_dyneither(_tmp898,sizeof(char),3));}case Cyc_Absyn_Not: _LL204: {const char*
_tmp899;return(_tmp899="!",_tag_dyneither(_tmp899,sizeof(char),2));}case Cyc_Absyn_Bitnot:
_LL205: {const char*_tmp89A;return(_tmp89A="~",_tag_dyneither(_tmp89A,sizeof(char),
2));}case Cyc_Absyn_Bitand: _LL206: {const char*_tmp89B;return(_tmp89B="&",
_tag_dyneither(_tmp89B,sizeof(char),2));}case Cyc_Absyn_Bitor: _LL207: {const char*
_tmp89C;return(_tmp89C="|",_tag_dyneither(_tmp89C,sizeof(char),2));}case Cyc_Absyn_Bitxor:
_LL208: {const char*_tmp89D;return(_tmp89D="^",_tag_dyneither(_tmp89D,sizeof(char),
2));}case Cyc_Absyn_Bitlshift: _LL209: {const char*_tmp89E;return(_tmp89E="<<",
_tag_dyneither(_tmp89E,sizeof(char),3));}case Cyc_Absyn_Bitlrshift: _LL20A: {const
char*_tmp89F;return(_tmp89F=">>",_tag_dyneither(_tmp89F,sizeof(char),3));}case
Cyc_Absyn_Bitarshift: _LL20B: {const char*_tmp8A0;return(_tmp8A0=">>>",
_tag_dyneither(_tmp8A0,sizeof(char),4));}case Cyc_Absyn_Numelts: _LL20C: {const
char*_tmp8A1;return(_tmp8A1="numelts",_tag_dyneither(_tmp8A1,sizeof(char),8));}}}
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p);struct Cyc_PP_Doc*
Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){return Cyc_PP_text(Cyc_Absynpp_prim2str(
p));}int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s){void*_tmp395=s->r;_LL20F: {struct Cyc_Absyn_Decl_s_struct*
_tmp396=(struct Cyc_Absyn_Decl_s_struct*)_tmp395;if(_tmp396->tag != 12)goto _LL211;}
_LL210: return 1;_LL211:;_LL212: return 0;_LL20E:;}struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(
struct Cyc_Absyn_Stmt*st,int stmtexp);struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct
Cyc_Absyn_Stmt*st,int stmtexp){struct Cyc_PP_Doc*s;{void*_tmp397=st->r;struct Cyc_Absyn_Exp*
_tmp39A;struct Cyc_Absyn_Stmt*_tmp39C;struct Cyc_Absyn_Stmt*_tmp39D;struct Cyc_Absyn_Exp*
_tmp39F;struct Cyc_Absyn_Exp*_tmp3A1;struct Cyc_Absyn_Stmt*_tmp3A2;struct Cyc_Absyn_Stmt*
_tmp3A3;struct _tuple8 _tmp3A5;struct Cyc_Absyn_Exp*_tmp3A6;struct Cyc_Absyn_Stmt*
_tmp3A7;struct _dyneither_ptr*_tmp3AB;struct Cyc_Absyn_Exp*_tmp3AD;struct _tuple8
_tmp3AE;struct Cyc_Absyn_Exp*_tmp3AF;struct _tuple8 _tmp3B0;struct Cyc_Absyn_Exp*
_tmp3B1;struct Cyc_Absyn_Stmt*_tmp3B2;struct Cyc_Absyn_Exp*_tmp3B4;struct Cyc_List_List*
_tmp3B5;struct Cyc_List_List*_tmp3B7;struct Cyc_List_List*_tmp3B9;struct Cyc_Absyn_Decl*
_tmp3BB;struct Cyc_Absyn_Stmt*_tmp3BC;struct _dyneither_ptr*_tmp3BE;struct Cyc_Absyn_Stmt*
_tmp3BF;struct Cyc_Absyn_Stmt*_tmp3C1;struct _tuple8 _tmp3C2;struct Cyc_Absyn_Exp*
_tmp3C3;struct Cyc_Absyn_Stmt*_tmp3C5;struct Cyc_List_List*_tmp3C6;struct Cyc_Absyn_Exp*
_tmp3C8;_LL214: {struct Cyc_Absyn_Skip_s_struct*_tmp398=(struct Cyc_Absyn_Skip_s_struct*)
_tmp397;if(_tmp398->tag != 0)goto _LL216;}_LL215:{const char*_tmp8A2;s=Cyc_PP_text(((
_tmp8A2=";",_tag_dyneither(_tmp8A2,sizeof(char),2))));}goto _LL213;_LL216: {
struct Cyc_Absyn_Exp_s_struct*_tmp399=(struct Cyc_Absyn_Exp_s_struct*)_tmp397;if(
_tmp399->tag != 1)goto _LL218;else{_tmp39A=_tmp399->f1;}}_LL217:{const char*_tmp8A5;
struct Cyc_PP_Doc*_tmp8A4[2];s=((_tmp8A4[1]=Cyc_PP_text(((_tmp8A5=";",
_tag_dyneither(_tmp8A5,sizeof(char),2)))),((_tmp8A4[0]=Cyc_Absynpp_exp2doc(
_tmp39A),Cyc_PP_cat(_tag_dyneither(_tmp8A4,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL213;_LL218: {struct Cyc_Absyn_Seq_s_struct*_tmp39B=(struct Cyc_Absyn_Seq_s_struct*)
_tmp397;if(_tmp39B->tag != 2)goto _LL21A;else{_tmp39C=_tmp39B->f1;_tmp39D=_tmp39B->f2;}}
_LL219: if(Cyc_Absynpp_decls_first){if(Cyc_Absynpp_is_declaration(_tmp39C)){
struct Cyc_PP_Doc*_tmp8B0[5];struct Cyc_PP_Doc*_tmp8AF[7];const char*_tmp8AE;const
char*_tmp8AD;struct Cyc_PP_Doc*_tmp8AC[7];s=((_tmp8AC[6]=Cyc_Absynpp_is_declaration(
_tmp39D)?stmtexp?(_tmp8AF[6]=Cyc_PP_line_doc(),((_tmp8AF[5]=Cyc_PP_text(((
_tmp8AE=");",_tag_dyneither(_tmp8AE,sizeof(char),3)))),((_tmp8AF[4]=Cyc_Absynpp_rb(),((
_tmp8AF[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp39D,stmtexp)),((_tmp8AF[2]=Cyc_PP_blank_doc(),((
_tmp8AF[1]=Cyc_Absynpp_lb(),((_tmp8AF[0]=Cyc_PP_text(((_tmp8AD="(",
_tag_dyneither(_tmp8AD,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8AF,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))):((_tmp8B0[4]=Cyc_PP_line_doc(),((
_tmp8B0[3]=Cyc_Absynpp_rb(),((_tmp8B0[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp39D,stmtexp)),((_tmp8B0[1]=Cyc_PP_blank_doc(),((_tmp8B0[0]=Cyc_Absynpp_lb(),
Cyc_PP_cat(_tag_dyneither(_tmp8B0,sizeof(struct Cyc_PP_Doc*),5)))))))))))): Cyc_Absynpp_stmt2doc(
_tmp39D,stmtexp),((_tmp8AC[5]=Cyc_PP_line_doc(),((_tmp8AC[4]=Cyc_Absynpp_rb(),((
_tmp8AC[3]=Cyc_PP_line_doc(),((_tmp8AC[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp39C,0)),((_tmp8AC[1]=Cyc_PP_blank_doc(),((_tmp8AC[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(
_tag_dyneither(_tmp8AC,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{if(Cyc_Absynpp_is_declaration(
_tmp39D)){struct Cyc_PP_Doc*_tmp8BB[4];struct Cyc_PP_Doc*_tmp8BA[6];const char*
_tmp8B9;const char*_tmp8B8;struct Cyc_PP_Doc*_tmp8B7[4];s=((_tmp8B7[3]=Cyc_PP_line_doc(),((
_tmp8B7[2]=stmtexp?(_tmp8BA[5]=Cyc_PP_text(((_tmp8B9=");",_tag_dyneither(_tmp8B9,
sizeof(char),3)))),((_tmp8BA[4]=Cyc_Absynpp_rb(),((_tmp8BA[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp39D,stmtexp)),((_tmp8BA[2]=Cyc_PP_blank_doc(),((_tmp8BA[1]=Cyc_Absynpp_lb(),((
_tmp8BA[0]=Cyc_PP_text(((_tmp8B8="(",_tag_dyneither(_tmp8B8,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp8BA,sizeof(struct Cyc_PP_Doc*),6))))))))))))):((
_tmp8BB[3]=Cyc_Absynpp_rb(),((_tmp8BB[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp39D,stmtexp)),((_tmp8BB[1]=Cyc_PP_blank_doc(),((_tmp8BB[0]=Cyc_Absynpp_lb(),
Cyc_PP_cat(_tag_dyneither(_tmp8BB,sizeof(struct Cyc_PP_Doc*),4)))))))))),((
_tmp8B7[1]=Cyc_PP_line_doc(),((_tmp8B7[0]=Cyc_Absynpp_stmt2doc(_tmp39C,0),Cyc_PP_cat(
_tag_dyneither(_tmp8B7,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{struct Cyc_PP_Doc*
_tmp8BC[3];s=((_tmp8BC[2]=Cyc_Absynpp_stmt2doc(_tmp39D,stmtexp),((_tmp8BC[1]=Cyc_PP_line_doc(),((
_tmp8BC[0]=Cyc_Absynpp_stmt2doc(_tmp39C,0),Cyc_PP_cat(_tag_dyneither(_tmp8BC,
sizeof(struct Cyc_PP_Doc*),3))))))));}}}else{struct Cyc_PP_Doc*_tmp8BD[3];s=((
_tmp8BD[2]=Cyc_Absynpp_stmt2doc(_tmp39D,stmtexp),((_tmp8BD[1]=Cyc_PP_line_doc(),((
_tmp8BD[0]=Cyc_Absynpp_stmt2doc(_tmp39C,0),Cyc_PP_cat(_tag_dyneither(_tmp8BD,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL213;_LL21A: {struct Cyc_Absyn_Return_s_struct*
_tmp39E=(struct Cyc_Absyn_Return_s_struct*)_tmp397;if(_tmp39E->tag != 3)goto _LL21C;
else{_tmp39F=_tmp39E->f1;}}_LL21B: if(_tmp39F == 0){const char*_tmp8BE;s=Cyc_PP_text(((
_tmp8BE="return;",_tag_dyneither(_tmp8BE,sizeof(char),8))));}else{const char*
_tmp8C3;const char*_tmp8C2;struct Cyc_PP_Doc*_tmp8C1[3];s=((_tmp8C1[2]=Cyc_PP_text(((
_tmp8C2=";",_tag_dyneither(_tmp8C2,sizeof(char),2)))),((_tmp8C1[1]=_tmp39F == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp39F),((_tmp8C1[0]=
Cyc_PP_text(((_tmp8C3="return ",_tag_dyneither(_tmp8C3,sizeof(char),8)))),Cyc_PP_cat(
_tag_dyneither(_tmp8C1,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL213;_LL21C: {
struct Cyc_Absyn_IfThenElse_s_struct*_tmp3A0=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp397;if(_tmp3A0->tag != 4)goto _LL21E;else{_tmp3A1=_tmp3A0->f1;_tmp3A2=_tmp3A0->f2;
_tmp3A3=_tmp3A0->f3;}}_LL21D: {int print_else;{void*_tmp3DC=_tmp3A3->r;_LL239: {
struct Cyc_Absyn_Skip_s_struct*_tmp3DD=(struct Cyc_Absyn_Skip_s_struct*)_tmp3DC;
if(_tmp3DD->tag != 0)goto _LL23B;}_LL23A: print_else=0;goto _LL238;_LL23B:;_LL23C:
print_else=1;goto _LL238;_LL238:;}{const char*_tmp8D2;const char*_tmp8D1;struct Cyc_PP_Doc*
_tmp8D0[2];struct Cyc_PP_Doc*_tmp8CF[6];struct Cyc_PP_Doc*_tmp8CE[2];const char*
_tmp8CD;struct Cyc_PP_Doc*_tmp8CC[8];s=((_tmp8CC[7]=print_else?(_tmp8CF[5]=Cyc_Absynpp_rb(),((
_tmp8CF[4]=Cyc_PP_line_doc(),((_tmp8CF[3]=Cyc_PP_nest(2,((_tmp8CE[1]=Cyc_Absynpp_stmt2doc(
_tmp3A3,0),((_tmp8CE[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8CE,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8CF[2]=Cyc_Absynpp_lb(),((_tmp8CF[1]=
Cyc_PP_text(((_tmp8CD="else ",_tag_dyneither(_tmp8CD,sizeof(char),6)))),((
_tmp8CF[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8CF,sizeof(struct Cyc_PP_Doc*),
6))))))))))))): Cyc_PP_nil_doc(),((_tmp8CC[6]=Cyc_Absynpp_rb(),((_tmp8CC[5]=Cyc_PP_line_doc(),((
_tmp8CC[4]=Cyc_PP_nest(2,((_tmp8D0[1]=Cyc_Absynpp_stmt2doc(_tmp3A2,0),((_tmp8D0[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8D0,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp8CC[3]=Cyc_Absynpp_lb(),((_tmp8CC[2]=Cyc_PP_text(((_tmp8D1=") ",
_tag_dyneither(_tmp8D1,sizeof(char),3)))),((_tmp8CC[1]=Cyc_Absynpp_exp2doc(
_tmp3A1),((_tmp8CC[0]=Cyc_PP_text(((_tmp8D2="if (",_tag_dyneither(_tmp8D2,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8CC,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL213;}_LL21E: {struct Cyc_Absyn_While_s_struct*_tmp3A4=(struct Cyc_Absyn_While_s_struct*)
_tmp397;if(_tmp3A4->tag != 5)goto _LL220;else{_tmp3A5=_tmp3A4->f1;_tmp3A6=_tmp3A5.f1;
_tmp3A7=_tmp3A4->f2;}}_LL21F:{const char*_tmp8D9;const char*_tmp8D8;struct Cyc_PP_Doc*
_tmp8D7[2];struct Cyc_PP_Doc*_tmp8D6[7];s=((_tmp8D6[6]=Cyc_Absynpp_rb(),((_tmp8D6[
5]=Cyc_PP_line_doc(),((_tmp8D6[4]=Cyc_PP_nest(2,((_tmp8D7[1]=Cyc_Absynpp_stmt2doc(
_tmp3A7,0),((_tmp8D7[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8D7,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8D6[3]=Cyc_Absynpp_lb(),((_tmp8D6[2]=
Cyc_PP_text(((_tmp8D8=") ",_tag_dyneither(_tmp8D8,sizeof(char),3)))),((_tmp8D6[1]=
Cyc_Absynpp_exp2doc(_tmp3A6),((_tmp8D6[0]=Cyc_PP_text(((_tmp8D9="while (",
_tag_dyneither(_tmp8D9,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp8D6,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}goto _LL213;_LL220: {struct Cyc_Absyn_Break_s_struct*
_tmp3A8=(struct Cyc_Absyn_Break_s_struct*)_tmp397;if(_tmp3A8->tag != 6)goto _LL222;}
_LL221:{const char*_tmp8DA;s=Cyc_PP_text(((_tmp8DA="break;",_tag_dyneither(
_tmp8DA,sizeof(char),7))));}goto _LL213;_LL222: {struct Cyc_Absyn_Continue_s_struct*
_tmp3A9=(struct Cyc_Absyn_Continue_s_struct*)_tmp397;if(_tmp3A9->tag != 7)goto
_LL224;}_LL223:{const char*_tmp8DB;s=Cyc_PP_text(((_tmp8DB="continue;",
_tag_dyneither(_tmp8DB,sizeof(char),10))));}goto _LL213;_LL224: {struct Cyc_Absyn_Goto_s_struct*
_tmp3AA=(struct Cyc_Absyn_Goto_s_struct*)_tmp397;if(_tmp3AA->tag != 8)goto _LL226;
else{_tmp3AB=_tmp3AA->f1;}}_LL225:{const char*_tmp8DF;void*_tmp8DE[1];struct Cyc_String_pa_struct
_tmp8DD;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp8DD.tag=0,((_tmp8DD.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp3AB),((_tmp8DE[0]=& _tmp8DD,Cyc_aprintf(((
_tmp8DF="goto %s;",_tag_dyneither(_tmp8DF,sizeof(char),9))),_tag_dyneither(
_tmp8DE,sizeof(void*),1)))))))));}goto _LL213;_LL226: {struct Cyc_Absyn_For_s_struct*
_tmp3AC=(struct Cyc_Absyn_For_s_struct*)_tmp397;if(_tmp3AC->tag != 9)goto _LL228;
else{_tmp3AD=_tmp3AC->f1;_tmp3AE=_tmp3AC->f2;_tmp3AF=_tmp3AE.f1;_tmp3B0=_tmp3AC->f3;
_tmp3B1=_tmp3B0.f1;_tmp3B2=_tmp3AC->f4;}}_LL227:{const char*_tmp8EA;const char*
_tmp8E9;const char*_tmp8E8;const char*_tmp8E7;struct Cyc_PP_Doc*_tmp8E6[2];struct
Cyc_PP_Doc*_tmp8E5[11];s=((_tmp8E5[10]=Cyc_Absynpp_rb(),((_tmp8E5[9]=Cyc_PP_line_doc(),((
_tmp8E5[8]=Cyc_PP_nest(2,((_tmp8E6[1]=Cyc_Absynpp_stmt2doc(_tmp3B2,0),((_tmp8E6[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8E6,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp8E5[7]=Cyc_Absynpp_lb(),((_tmp8E5[6]=Cyc_PP_text(((_tmp8E7=") ",
_tag_dyneither(_tmp8E7,sizeof(char),3)))),((_tmp8E5[5]=Cyc_Absynpp_exp2doc(
_tmp3B1),((_tmp8E5[4]=Cyc_PP_text(((_tmp8E8="; ",_tag_dyneither(_tmp8E8,sizeof(
char),3)))),((_tmp8E5[3]=Cyc_Absynpp_exp2doc(_tmp3AF),((_tmp8E5[2]=Cyc_PP_text(((
_tmp8E9="; ",_tag_dyneither(_tmp8E9,sizeof(char),3)))),((_tmp8E5[1]=Cyc_Absynpp_exp2doc(
_tmp3AD),((_tmp8E5[0]=Cyc_PP_text(((_tmp8EA="for(",_tag_dyneither(_tmp8EA,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8E5,sizeof(struct Cyc_PP_Doc*),
11))))))))))))))))))))))));}goto _LL213;_LL228: {struct Cyc_Absyn_Switch_s_struct*
_tmp3B3=(struct Cyc_Absyn_Switch_s_struct*)_tmp397;if(_tmp3B3->tag != 10)goto
_LL22A;else{_tmp3B4=_tmp3B3->f1;_tmp3B5=_tmp3B3->f2;}}_LL229:{const char*_tmp8EF;
const char*_tmp8EE;struct Cyc_PP_Doc*_tmp8ED[8];s=((_tmp8ED[7]=Cyc_Absynpp_rb(),((
_tmp8ED[6]=Cyc_PP_line_doc(),((_tmp8ED[5]=Cyc_Absynpp_switchclauses2doc(_tmp3B5),((
_tmp8ED[4]=Cyc_PP_line_doc(),((_tmp8ED[3]=Cyc_Absynpp_lb(),((_tmp8ED[2]=Cyc_PP_text(((
_tmp8EE=") ",_tag_dyneither(_tmp8EE,sizeof(char),3)))),((_tmp8ED[1]=Cyc_Absynpp_exp2doc(
_tmp3B4),((_tmp8ED[0]=Cyc_PP_text(((_tmp8EF="switch (",_tag_dyneither(_tmp8EF,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp8ED,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL213;_LL22A: {struct Cyc_Absyn_Fallthru_s_struct*_tmp3B6=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp397;if(_tmp3B6->tag != 11)goto _LL22C;else{_tmp3B7=_tmp3B6->f1;if(_tmp3B7 != 0)
goto _LL22C;}}_LL22B:{const char*_tmp8F0;s=Cyc_PP_text(((_tmp8F0="fallthru;",
_tag_dyneither(_tmp8F0,sizeof(char),10))));}goto _LL213;_LL22C: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp3B8=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp397;if(_tmp3B8->tag != 11)goto
_LL22E;else{_tmp3B9=_tmp3B8->f1;}}_LL22D:{const char*_tmp8F5;const char*_tmp8F4;
struct Cyc_PP_Doc*_tmp8F3[3];s=((_tmp8F3[2]=Cyc_PP_text(((_tmp8F4=");",
_tag_dyneither(_tmp8F4,sizeof(char),3)))),((_tmp8F3[1]=Cyc_Absynpp_exps2doc_prec(
20,_tmp3B9),((_tmp8F3[0]=Cyc_PP_text(((_tmp8F5="fallthru(",_tag_dyneither(
_tmp8F5,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp8F3,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LL213;_LL22E: {struct Cyc_Absyn_Decl_s_struct*_tmp3BA=(struct Cyc_Absyn_Decl_s_struct*)
_tmp397;if(_tmp3BA->tag != 12)goto _LL230;else{_tmp3BB=_tmp3BA->f1;_tmp3BC=_tmp3BA->f2;}}
_LL22F:{struct Cyc_PP_Doc*_tmp8F6[3];s=((_tmp8F6[2]=Cyc_Absynpp_stmt2doc(_tmp3BC,
stmtexp),((_tmp8F6[1]=Cyc_PP_line_doc(),((_tmp8F6[0]=Cyc_Absynpp_decl2doc(
_tmp3BB),Cyc_PP_cat(_tag_dyneither(_tmp8F6,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL213;_LL230: {struct Cyc_Absyn_Label_s_struct*_tmp3BD=(struct Cyc_Absyn_Label_s_struct*)
_tmp397;if(_tmp3BD->tag != 13)goto _LL232;else{_tmp3BE=_tmp3BD->f1;_tmp3BF=_tmp3BD->f2;}}
_LL231: if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp3BF)){if(
stmtexp){const char*_tmp8FB;const char*_tmp8FA;struct Cyc_PP_Doc*_tmp8F9[8];s=((
_tmp8F9[7]=Cyc_PP_text(((_tmp8FA=");",_tag_dyneither(_tmp8FA,sizeof(char),3)))),((
_tmp8F9[6]=Cyc_Absynpp_rb(),((_tmp8F9[5]=Cyc_PP_line_doc(),((_tmp8F9[4]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp3BF,1)),((_tmp8F9[3]=Cyc_PP_line_doc(),((_tmp8F9[2]=
Cyc_Absynpp_lb(),((_tmp8F9[1]=Cyc_PP_text(((_tmp8FB=": (",_tag_dyneither(_tmp8FB,
sizeof(char),4)))),((_tmp8F9[0]=Cyc_PP_textptr(_tmp3BE),Cyc_PP_cat(
_tag_dyneither(_tmp8F9,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{const
char*_tmp8FE;struct Cyc_PP_Doc*_tmp8FD[7];s=((_tmp8FD[6]=Cyc_Absynpp_rb(),((
_tmp8FD[5]=Cyc_PP_line_doc(),((_tmp8FD[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3BF,0)),((_tmp8FD[3]=Cyc_PP_line_doc(),((_tmp8FD[2]=Cyc_Absynpp_lb(),((
_tmp8FD[1]=Cyc_PP_text(((_tmp8FE=": ",_tag_dyneither(_tmp8FE,sizeof(char),3)))),((
_tmp8FD[0]=Cyc_PP_textptr(_tmp3BE),Cyc_PP_cat(_tag_dyneither(_tmp8FD,sizeof(
struct Cyc_PP_Doc*),7))))))))))))))));}}else{const char*_tmp901;struct Cyc_PP_Doc*
_tmp900[3];s=((_tmp900[2]=Cyc_Absynpp_stmt2doc(_tmp3BF,stmtexp),((_tmp900[1]=Cyc_PP_text(((
_tmp901=": ",_tag_dyneither(_tmp901,sizeof(char),3)))),((_tmp900[0]=Cyc_PP_textptr(
_tmp3BE),Cyc_PP_cat(_tag_dyneither(_tmp900,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL213;_LL232: {struct Cyc_Absyn_Do_s_struct*_tmp3C0=(struct Cyc_Absyn_Do_s_struct*)
_tmp397;if(_tmp3C0->tag != 14)goto _LL234;else{_tmp3C1=_tmp3C0->f1;_tmp3C2=_tmp3C0->f2;
_tmp3C3=_tmp3C2.f1;}}_LL233:{const char*_tmp908;const char*_tmp907;const char*
_tmp906;struct Cyc_PP_Doc*_tmp905[9];s=((_tmp905[8]=Cyc_PP_text(((_tmp906=");",
_tag_dyneither(_tmp906,sizeof(char),3)))),((_tmp905[7]=Cyc_Absynpp_exp2doc(
_tmp3C3),((_tmp905[6]=Cyc_PP_text(((_tmp907=" while (",_tag_dyneither(_tmp907,
sizeof(char),9)))),((_tmp905[5]=Cyc_Absynpp_rb(),((_tmp905[4]=Cyc_PP_line_doc(),((
_tmp905[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3C1,0)),((_tmp905[2]=Cyc_PP_line_doc(),((
_tmp905[1]=Cyc_Absynpp_lb(),((_tmp905[0]=Cyc_PP_text(((_tmp908="do ",
_tag_dyneither(_tmp908,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp905,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL213;_LL234: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp3C4=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp397;if(_tmp3C4->tag != 15)goto
_LL236;else{_tmp3C5=_tmp3C4->f1;_tmp3C6=_tmp3C4->f2;}}_LL235:{const char*_tmp90D;
const char*_tmp90C;struct Cyc_PP_Doc*_tmp90B[12];s=((_tmp90B[11]=Cyc_Absynpp_rb(),((
_tmp90B[10]=Cyc_PP_line_doc(),((_tmp90B[9]=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(
_tmp3C6)),((_tmp90B[8]=Cyc_PP_line_doc(),((_tmp90B[7]=Cyc_Absynpp_lb(),((_tmp90B[
6]=Cyc_PP_text(((_tmp90C=" catch ",_tag_dyneither(_tmp90C,sizeof(char),8)))),((
_tmp90B[5]=Cyc_Absynpp_rb(),((_tmp90B[4]=Cyc_PP_line_doc(),((_tmp90B[3]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp3C5,0)),((_tmp90B[2]=Cyc_PP_line_doc(),((_tmp90B[1]=
Cyc_Absynpp_lb(),((_tmp90B[0]=Cyc_PP_text(((_tmp90D="try ",_tag_dyneither(
_tmp90D,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp90B,sizeof(struct Cyc_PP_Doc*),
12))))))))))))))))))))))))));}goto _LL213;_LL236: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp3C7=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp397;if(_tmp3C7->tag != 16)goto
_LL213;else{_tmp3C8=_tmp3C7->f1;}}_LL237:{const char*_tmp912;const char*_tmp911;
struct Cyc_PP_Doc*_tmp910[3];s=((_tmp910[2]=Cyc_PP_text(((_tmp911=");",
_tag_dyneither(_tmp911,sizeof(char),3)))),((_tmp910[1]=Cyc_Absynpp_exp2doc(
_tmp3C8),((_tmp910[0]=Cyc_PP_text(((_tmp912="reset_region(",_tag_dyneither(
_tmp912,sizeof(char),14)))),Cyc_PP_cat(_tag_dyneither(_tmp910,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LL213;_LL213:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat*p);struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*
p){struct Cyc_PP_Doc*s;{void*_tmp40D=p->r;enum Cyc_Absyn_Sign _tmp411;int _tmp412;
char _tmp414;struct _dyneither_ptr _tmp416;struct Cyc_Absyn_Vardecl*_tmp418;struct
Cyc_Absyn_Pat*_tmp419;struct Cyc_Absyn_Pat _tmp41A;void*_tmp41B;struct Cyc_Absyn_Vardecl*
_tmp41E;struct Cyc_Absyn_Pat*_tmp41F;struct Cyc_Absyn_Tvar*_tmp421;struct Cyc_Absyn_Vardecl*
_tmp422;struct Cyc_List_List*_tmp424;int _tmp425;struct Cyc_Absyn_Pat*_tmp427;
struct Cyc_Absyn_Vardecl*_tmp429;struct Cyc_Absyn_Pat*_tmp42A;struct Cyc_Absyn_Pat
_tmp42B;void*_tmp42C;struct Cyc_Absyn_Vardecl*_tmp42F;struct Cyc_Absyn_Pat*_tmp430;
struct _tuple0*_tmp432;struct _tuple0*_tmp434;struct Cyc_List_List*_tmp435;int
_tmp436;struct Cyc_Absyn_AggrInfo*_tmp438;struct Cyc_Absyn_AggrInfo _tmp439;union
Cyc_Absyn_AggrInfoU _tmp43A;struct Cyc_List_List*_tmp43B;struct Cyc_List_List*
_tmp43C;int _tmp43D;struct Cyc_Absyn_AggrInfo*_tmp43F;struct Cyc_List_List*_tmp440;
struct Cyc_List_List*_tmp441;int _tmp442;struct Cyc_Absyn_Enumfield*_tmp444;struct
Cyc_Absyn_Enumfield*_tmp446;struct Cyc_Absyn_Datatypefield*_tmp448;struct Cyc_List_List*
_tmp449;struct Cyc_Absyn_Datatypefield*_tmp44B;struct Cyc_List_List*_tmp44C;int
_tmp44D;struct Cyc_Absyn_Exp*_tmp44F;_LL23E: {struct Cyc_Absyn_Wild_p_struct*
_tmp40E=(struct Cyc_Absyn_Wild_p_struct*)_tmp40D;if(_tmp40E->tag != 0)goto _LL240;}
_LL23F:{const char*_tmp913;s=Cyc_PP_text(((_tmp913="_",_tag_dyneither(_tmp913,
sizeof(char),2))));}goto _LL23D;_LL240: {struct Cyc_Absyn_Null_p_struct*_tmp40F=(
struct Cyc_Absyn_Null_p_struct*)_tmp40D;if(_tmp40F->tag != 8)goto _LL242;}_LL241:{
const char*_tmp914;s=Cyc_PP_text(((_tmp914="NULL",_tag_dyneither(_tmp914,sizeof(
char),5))));}goto _LL23D;_LL242: {struct Cyc_Absyn_Int_p_struct*_tmp410=(struct Cyc_Absyn_Int_p_struct*)
_tmp40D;if(_tmp410->tag != 9)goto _LL244;else{_tmp411=_tmp410->f1;_tmp412=_tmp410->f2;}}
_LL243: if(_tmp411 != (enum Cyc_Absyn_Sign)Cyc_Absyn_Unsigned){const char*_tmp918;
void*_tmp917[1];struct Cyc_Int_pa_struct _tmp916;s=Cyc_PP_text((struct
_dyneither_ptr)((_tmp916.tag=1,((_tmp916.f1=(unsigned long)_tmp412,((_tmp917[0]=&
_tmp916,Cyc_aprintf(((_tmp918="%d",_tag_dyneither(_tmp918,sizeof(char),3))),
_tag_dyneither(_tmp917,sizeof(void*),1)))))))));}else{const char*_tmp91C;void*
_tmp91B[1];struct Cyc_Int_pa_struct _tmp91A;s=Cyc_PP_text((struct _dyneither_ptr)((
_tmp91A.tag=1,((_tmp91A.f1=(unsigned int)_tmp412,((_tmp91B[0]=& _tmp91A,Cyc_aprintf(((
_tmp91C="%u",_tag_dyneither(_tmp91C,sizeof(char),3))),_tag_dyneither(_tmp91B,
sizeof(void*),1)))))))));}goto _LL23D;_LL244: {struct Cyc_Absyn_Char_p_struct*
_tmp413=(struct Cyc_Absyn_Char_p_struct*)_tmp40D;if(_tmp413->tag != 10)goto _LL246;
else{_tmp414=_tmp413->f1;}}_LL245:{const char*_tmp920;void*_tmp91F[1];struct Cyc_String_pa_struct
_tmp91E;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp91E.tag=0,((_tmp91E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp414)),((
_tmp91F[0]=& _tmp91E,Cyc_aprintf(((_tmp920="'%s'",_tag_dyneither(_tmp920,sizeof(
char),5))),_tag_dyneither(_tmp91F,sizeof(void*),1)))))))));}goto _LL23D;_LL246: {
struct Cyc_Absyn_Float_p_struct*_tmp415=(struct Cyc_Absyn_Float_p_struct*)_tmp40D;
if(_tmp415->tag != 11)goto _LL248;else{_tmp416=_tmp415->f1;}}_LL247: s=Cyc_PP_text(
_tmp416);goto _LL23D;_LL248: {struct Cyc_Absyn_Var_p_struct*_tmp417=(struct Cyc_Absyn_Var_p_struct*)
_tmp40D;if(_tmp417->tag != 1)goto _LL24A;else{_tmp418=_tmp417->f1;_tmp419=_tmp417->f2;
_tmp41A=*_tmp419;_tmp41B=_tmp41A.r;{struct Cyc_Absyn_Wild_p_struct*_tmp41C=(
struct Cyc_Absyn_Wild_p_struct*)_tmp41B;if(_tmp41C->tag != 0)goto _LL24A;};}}_LL249:
s=Cyc_Absynpp_qvar2doc(_tmp418->name);goto _LL23D;_LL24A: {struct Cyc_Absyn_Var_p_struct*
_tmp41D=(struct Cyc_Absyn_Var_p_struct*)_tmp40D;if(_tmp41D->tag != 1)goto _LL24C;
else{_tmp41E=_tmp41D->f1;_tmp41F=_tmp41D->f2;}}_LL24B:{const char*_tmp923;struct
Cyc_PP_Doc*_tmp922[3];s=((_tmp922[2]=Cyc_Absynpp_pat2doc(_tmp41F),((_tmp922[1]=
Cyc_PP_text(((_tmp923=" as ",_tag_dyneither(_tmp923,sizeof(char),5)))),((_tmp922[
0]=Cyc_Absynpp_qvar2doc(_tmp41E->name),Cyc_PP_cat(_tag_dyneither(_tmp922,sizeof(
struct Cyc_PP_Doc*),3))))))));}goto _LL23D;_LL24C: {struct Cyc_Absyn_TagInt_p_struct*
_tmp420=(struct Cyc_Absyn_TagInt_p_struct*)_tmp40D;if(_tmp420->tag != 3)goto _LL24E;
else{_tmp421=_tmp420->f1;_tmp422=_tmp420->f2;}}_LL24D:{const char*_tmp928;const
char*_tmp927;struct Cyc_PP_Doc*_tmp926[4];s=((_tmp926[3]=Cyc_PP_text(((_tmp927=">",
_tag_dyneither(_tmp927,sizeof(char),2)))),((_tmp926[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(
_tmp421)),((_tmp926[1]=Cyc_PP_text(((_tmp928="<",_tag_dyneither(_tmp928,sizeof(
char),2)))),((_tmp926[0]=Cyc_Absynpp_qvar2doc(_tmp422->name),Cyc_PP_cat(
_tag_dyneither(_tmp926,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL23D;_LL24E: {
struct Cyc_Absyn_Tuple_p_struct*_tmp423=(struct Cyc_Absyn_Tuple_p_struct*)_tmp40D;
if(_tmp423->tag != 4)goto _LL250;else{_tmp424=_tmp423->f1;_tmp425=_tmp423->f2;}}
_LL24F:{const char*_tmp931;const char*_tmp930;const char*_tmp92F;const char*_tmp92E;
struct Cyc_PP_Doc*_tmp92D[4];s=((_tmp92D[3]=_tmp425?Cyc_PP_text(((_tmp92E=", ...)",
_tag_dyneither(_tmp92E,sizeof(char),7)))): Cyc_PP_text(((_tmp92F=")",
_tag_dyneither(_tmp92F,sizeof(char),2)))),((_tmp92D[2]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,((_tmp930=",",_tag_dyneither(_tmp930,
sizeof(char),2))),_tmp424),((_tmp92D[1]=Cyc_PP_text(((_tmp931="(",_tag_dyneither(
_tmp931,sizeof(char),2)))),((_tmp92D[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(
_tag_dyneither(_tmp92D,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL23D;_LL250: {
struct Cyc_Absyn_Pointer_p_struct*_tmp426=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp40D;if(_tmp426->tag != 5)goto _LL252;else{_tmp427=_tmp426->f1;}}_LL251:{const
char*_tmp934;struct Cyc_PP_Doc*_tmp933[2];s=((_tmp933[1]=Cyc_Absynpp_pat2doc(
_tmp427),((_tmp933[0]=Cyc_PP_text(((_tmp934="&",_tag_dyneither(_tmp934,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp933,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL23D;_LL252: {struct Cyc_Absyn_Reference_p_struct*_tmp428=(struct Cyc_Absyn_Reference_p_struct*)
_tmp40D;if(_tmp428->tag != 2)goto _LL254;else{_tmp429=_tmp428->f1;_tmp42A=_tmp428->f2;
_tmp42B=*_tmp42A;_tmp42C=_tmp42B.r;{struct Cyc_Absyn_Wild_p_struct*_tmp42D=(
struct Cyc_Absyn_Wild_p_struct*)_tmp42C;if(_tmp42D->tag != 0)goto _LL254;};}}_LL253:{
const char*_tmp937;struct Cyc_PP_Doc*_tmp936[2];s=((_tmp936[1]=Cyc_Absynpp_qvar2doc(
_tmp429->name),((_tmp936[0]=Cyc_PP_text(((_tmp937="*",_tag_dyneither(_tmp937,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp936,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL23D;_LL254: {struct Cyc_Absyn_Reference_p_struct*_tmp42E=(struct Cyc_Absyn_Reference_p_struct*)
_tmp40D;if(_tmp42E->tag != 2)goto _LL256;else{_tmp42F=_tmp42E->f1;_tmp430=_tmp42E->f2;}}
_LL255:{const char*_tmp93C;const char*_tmp93B;struct Cyc_PP_Doc*_tmp93A[4];s=((
_tmp93A[3]=Cyc_Absynpp_pat2doc(_tmp430),((_tmp93A[2]=Cyc_PP_text(((_tmp93B=" as ",
_tag_dyneither(_tmp93B,sizeof(char),5)))),((_tmp93A[1]=Cyc_Absynpp_qvar2doc(
_tmp42F->name),((_tmp93A[0]=Cyc_PP_text(((_tmp93C="*",_tag_dyneither(_tmp93C,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp93A,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL23D;_LL256: {struct Cyc_Absyn_UnknownId_p_struct*_tmp431=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp40D;if(_tmp431->tag != 14)goto _LL258;else{_tmp432=_tmp431->f1;}}_LL257: s=Cyc_Absynpp_qvar2doc(
_tmp432);goto _LL23D;_LL258: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp433=(
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp40D;if(_tmp433->tag != 15)goto _LL25A;
else{_tmp434=_tmp433->f1;_tmp435=_tmp433->f2;_tmp436=_tmp433->f3;}}_LL259: {
const char*_tmp93E;const char*_tmp93D;struct _dyneither_ptr term=_tmp436?(_tmp93E=", ...)",
_tag_dyneither(_tmp93E,sizeof(char),7)):((_tmp93D=")",_tag_dyneither(_tmp93D,
sizeof(char),2)));{const char*_tmp943;const char*_tmp942;struct Cyc_PP_Doc*_tmp941[
2];s=((_tmp941[1]=Cyc_PP_group(((_tmp943="(",_tag_dyneither(_tmp943,sizeof(char),
2))),term,((_tmp942=",",_tag_dyneither(_tmp942,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_pat2doc,_tmp435)),((_tmp941[0]=Cyc_Absynpp_qvar2doc(_tmp434),Cyc_PP_cat(
_tag_dyneither(_tmp941,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL23D;}_LL25A: {
struct Cyc_Absyn_Aggr_p_struct*_tmp437=(struct Cyc_Absyn_Aggr_p_struct*)_tmp40D;
if(_tmp437->tag != 6)goto _LL25C;else{_tmp438=_tmp437->f1;if(_tmp438 == 0)goto
_LL25C;_tmp439=*_tmp438;_tmp43A=_tmp439.aggr_info;_tmp43B=_tmp437->f2;_tmp43C=
_tmp437->f3;_tmp43D=_tmp437->f4;}}_LL25B: {const char*_tmp945;const char*_tmp944;
struct _dyneither_ptr term=_tmp43D?(_tmp945=", ...}",_tag_dyneither(_tmp945,
sizeof(char),7)):((_tmp944="}",_tag_dyneither(_tmp944,sizeof(char),2)));struct
_tuple0*_tmp472;struct _tuple9 _tmp471=Cyc_Absyn_aggr_kinded_name(_tmp43A);_tmp472=
_tmp471.f2;{const char*_tmp950;const char*_tmp94F;const char*_tmp94E;const char*
_tmp94D;const char*_tmp94C;struct Cyc_PP_Doc*_tmp94B[4];s=((_tmp94B[3]=Cyc_PP_group(((
_tmp94D="",_tag_dyneither(_tmp94D,sizeof(char),1))),term,((_tmp94C=",",
_tag_dyneither(_tmp94C,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp43C)),((_tmp94B[2]=Cyc_PP_egroup(((_tmp950="[",_tag_dyneither(_tmp950,
sizeof(char),2))),((_tmp94F="]",_tag_dyneither(_tmp94F,sizeof(char),2))),((
_tmp94E=",",_tag_dyneither(_tmp94E,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp43B))),((_tmp94B[1]=
Cyc_Absynpp_lb(),((_tmp94B[0]=Cyc_Absynpp_qvar2doc(_tmp472),Cyc_PP_cat(
_tag_dyneither(_tmp94B,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL23D;}_LL25C: {
struct Cyc_Absyn_Aggr_p_struct*_tmp43E=(struct Cyc_Absyn_Aggr_p_struct*)_tmp40D;
if(_tmp43E->tag != 6)goto _LL25E;else{_tmp43F=_tmp43E->f1;if(_tmp43F != 0)goto
_LL25E;_tmp440=_tmp43E->f2;_tmp441=_tmp43E->f3;_tmp442=_tmp43E->f4;}}_LL25D: {
const char*_tmp952;const char*_tmp951;struct _dyneither_ptr term=_tmp442?(_tmp952=", ...}",
_tag_dyneither(_tmp952,sizeof(char),7)):((_tmp951="}",_tag_dyneither(_tmp951,
sizeof(char),2)));{const char*_tmp95D;const char*_tmp95C;const char*_tmp95B;const
char*_tmp95A;const char*_tmp959;struct Cyc_PP_Doc*_tmp958[3];s=((_tmp958[2]=Cyc_PP_group(((
_tmp95A="",_tag_dyneither(_tmp95A,sizeof(char),1))),term,((_tmp959=",",
_tag_dyneither(_tmp959,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp441)),((_tmp958[1]=Cyc_PP_egroup(((_tmp95D="[",_tag_dyneither(_tmp95D,
sizeof(char),2))),((_tmp95C="]",_tag_dyneither(_tmp95C,sizeof(char),2))),((
_tmp95B=",",_tag_dyneither(_tmp95B,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp440))),((_tmp958[0]=
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp958,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL23D;}_LL25E: {struct Cyc_Absyn_Enum_p_struct*_tmp443=(struct Cyc_Absyn_Enum_p_struct*)
_tmp40D;if(_tmp443->tag != 12)goto _LL260;else{_tmp444=_tmp443->f2;}}_LL25F:
_tmp446=_tmp444;goto _LL261;_LL260: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp445=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp40D;if(_tmp445->tag != 13)goto _LL262;else{
_tmp446=_tmp445->f2;}}_LL261: s=Cyc_Absynpp_qvar2doc(_tmp446->name);goto _LL23D;
_LL262: {struct Cyc_Absyn_Datatype_p_struct*_tmp447=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp40D;if(_tmp447->tag != 7)goto _LL264;else{_tmp448=_tmp447->f2;_tmp449=_tmp447->f3;
if(_tmp449 != 0)goto _LL264;}}_LL263: s=Cyc_Absynpp_qvar2doc(_tmp448->name);goto
_LL23D;_LL264: {struct Cyc_Absyn_Datatype_p_struct*_tmp44A=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp40D;if(_tmp44A->tag != 7)goto _LL266;else{_tmp44B=_tmp44A->f2;_tmp44C=_tmp44A->f3;
_tmp44D=_tmp44A->f4;}}_LL265: {const char*_tmp95F;const char*_tmp95E;struct
_dyneither_ptr term=_tmp44D?(_tmp95F=", ...)",_tag_dyneither(_tmp95F,sizeof(char),
7)):((_tmp95E=")",_tag_dyneither(_tmp95E,sizeof(char),2)));{const char*_tmp964;
const char*_tmp963;struct Cyc_PP_Doc*_tmp962[2];s=((_tmp962[1]=Cyc_PP_egroup(((
_tmp964="(",_tag_dyneither(_tmp964,sizeof(char),2))),term,((_tmp963=",",
_tag_dyneither(_tmp963,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,
_tmp44C)),((_tmp962[0]=Cyc_Absynpp_qvar2doc(_tmp44B->name),Cyc_PP_cat(
_tag_dyneither(_tmp962,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL23D;}_LL266: {
struct Cyc_Absyn_Exp_p_struct*_tmp44E=(struct Cyc_Absyn_Exp_p_struct*)_tmp40D;if(
_tmp44E->tag != 16)goto _LL23D;else{_tmp44F=_tmp44E->f1;}}_LL267: s=Cyc_Absynpp_exp2doc(
_tmp44F);goto _LL23D;_LL23D:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct
_tuple13*dp);struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp){const char*
_tmp96B;const char*_tmp96A;const char*_tmp969;struct Cyc_PP_Doc*_tmp968[2];return(
_tmp968[1]=Cyc_Absynpp_pat2doc((*dp).f2),((_tmp968[0]=Cyc_PP_egroup(((_tmp96B="",
_tag_dyneither(_tmp96B,sizeof(char),1))),((_tmp96A="=",_tag_dyneither(_tmp96A,
sizeof(char),2))),((_tmp969="=",_tag_dyneither(_tmp969,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*dp).f1)),Cyc_PP_cat(_tag_dyneither(_tmp968,sizeof(
struct Cyc_PP_Doc*),2)))));}struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct
Cyc_Absyn_Switch_clause*c);struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct
Cyc_Absyn_Switch_clause*c){if(c->where_clause == 0  && (c->pattern)->r == (void*)&
Cyc_Absyn_Wild_p_val){const char*_tmp970;struct Cyc_PP_Doc*_tmp96F[2];struct Cyc_PP_Doc*
_tmp96E[2];return(_tmp96E[1]=Cyc_PP_nest(2,((_tmp96F[1]=Cyc_Absynpp_stmt2doc(c->body,
0),((_tmp96F[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp96F,sizeof(
struct Cyc_PP_Doc*),2))))))),((_tmp96E[0]=Cyc_PP_text(((_tmp970="default: ",
_tag_dyneither(_tmp970,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp96E,
sizeof(struct Cyc_PP_Doc*),2)))));}else{if(c->where_clause == 0){const char*_tmp977;
const char*_tmp976;struct Cyc_PP_Doc*_tmp975[2];struct Cyc_PP_Doc*_tmp974[4];return(
_tmp974[3]=Cyc_PP_nest(2,((_tmp975[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp975[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp975,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp974[2]=Cyc_PP_text(((_tmp976=": ",_tag_dyneither(_tmp976,sizeof(
char),3)))),((_tmp974[1]=Cyc_Absynpp_pat2doc(c->pattern),((_tmp974[0]=Cyc_PP_text(((
_tmp977="case ",_tag_dyneither(_tmp977,sizeof(char),6)))),Cyc_PP_cat(
_tag_dyneither(_tmp974,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{const char*
_tmp980;const char*_tmp97F;const char*_tmp97E;struct Cyc_PP_Doc*_tmp97D[2];struct
Cyc_PP_Doc*_tmp97C[6];return(_tmp97C[5]=Cyc_PP_nest(2,((_tmp97D[1]=Cyc_Absynpp_stmt2doc(
c->body,0),((_tmp97D[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp97D,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp97C[4]=Cyc_PP_text(((_tmp97E=": ",
_tag_dyneither(_tmp97E,sizeof(char),3)))),((_tmp97C[3]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(c->where_clause)),((_tmp97C[2]=Cyc_PP_text(((
_tmp97F=" && ",_tag_dyneither(_tmp97F,sizeof(char),5)))),((_tmp97C[1]=Cyc_Absynpp_pat2doc(
c->pattern),((_tmp97C[0]=Cyc_PP_text(((_tmp980="case ",_tag_dyneither(_tmp980,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp97C,sizeof(struct Cyc_PP_Doc*),6)))))))))))));}}}
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs);struct Cyc_PP_Doc*
Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){const char*_tmp981;return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,((
_tmp981="",_tag_dyneither(_tmp981,sizeof(char),1))),cs);}struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(
struct Cyc_Absyn_Enumfield*f);struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct
Cyc_Absyn_Enumfield*f){if(f->tag == 0)return Cyc_Absynpp_qvar2doc(f->name);else{
const char*_tmp984;struct Cyc_PP_Doc*_tmp983[3];return(_tmp983[2]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->tag)),((_tmp983[1]=Cyc_PP_text(((_tmp984=" = ",
_tag_dyneither(_tmp984,sizeof(char),4)))),((_tmp983[0]=Cyc_Absynpp_qvar2doc(f->name),
Cyc_PP_cat(_tag_dyneither(_tmp983,sizeof(struct Cyc_PP_Doc*),3)))))));}}struct Cyc_PP_Doc*
Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs);struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(
struct Cyc_List_List*fs){const char*_tmp985;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))
Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,((_tmp985=",",_tag_dyneither(_tmp985,
sizeof(char),2))),fs);}static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*
v);static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){return
Cyc_Absynpp_qvar2doc(v->name);}static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct
Cyc_List_List*vds);static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*
vds){const char*_tmp986;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(
Cyc_Absynpp_id2doc,((_tmp986=",",_tag_dyneither(_tmp986,sizeof(char),2))),vds);}
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd);struct Cyc_PP_Doc*
Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){struct Cyc_Absyn_Vardecl
_tmp49E;enum Cyc_Absyn_Scope _tmp49F;struct _tuple0*_tmp4A0;struct Cyc_Absyn_Tqual
_tmp4A1;void*_tmp4A2;struct Cyc_Absyn_Exp*_tmp4A3;struct Cyc_List_List*_tmp4A4;
struct Cyc_Absyn_Vardecl*_tmp49D=vd;_tmp49E=*_tmp49D;_tmp49F=_tmp49E.sc;_tmp4A0=
_tmp49E.name;_tmp4A1=_tmp49E.tq;_tmp4A2=_tmp49E.type;_tmp4A3=_tmp49E.initializer;
_tmp4A4=_tmp49E.attributes;{struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(
_tmp4A0);struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4A4);struct Cyc_PP_Doc*
beforenamedoc;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL268:
beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL269:{void*_tmp4A5=Cyc_Tcutil_compress(
_tmp4A2);struct Cyc_Absyn_FnInfo _tmp4A7;struct Cyc_List_List*_tmp4A8;_LL26C: {
struct Cyc_Absyn_FnType_struct*_tmp4A6=(struct Cyc_Absyn_FnType_struct*)_tmp4A5;
if(_tmp4A6->tag != 10)goto _LL26E;else{_tmp4A7=_tmp4A6->f1;_tmp4A8=_tmp4A7.attributes;}}
_LL26D: beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp4A8);goto _LL26B;_LL26E:;_LL26F:
beforenamedoc=Cyc_PP_nil_doc();goto _LL26B;_LL26B:;}break;}{struct Cyc_PP_Doc*
tmp_doc;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL270: tmp_doc=Cyc_PP_nil_doc();
break;case Cyc_Cyclone_Vc_c: _LL271: tmp_doc=attsdoc;break;}{struct Cyc_Core_Opt*
_tmp993;struct Cyc_PP_Doc*_tmp992[2];struct Cyc_PP_Doc*_tmp991[2];const char*
_tmp990;const char*_tmp98F;struct Cyc_PP_Doc*_tmp98E[5];s=((_tmp98E[4]=Cyc_PP_text(((
_tmp98F=";",_tag_dyneither(_tmp98F,sizeof(char),2)))),((_tmp98E[3]=_tmp4A3 == 0?
Cyc_PP_nil_doc():((_tmp991[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp4A3),((
_tmp991[0]=Cyc_PP_text(((_tmp990=" = ",_tag_dyneither(_tmp990,sizeof(char),4)))),
Cyc_PP_cat(_tag_dyneither(_tmp991,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp98E[2]=
Cyc_Absynpp_tqtd2doc(_tmp4A1,_tmp4A2,((_tmp993=_cycalloc(sizeof(*_tmp993)),((
_tmp993->v=((_tmp992[1]=sn,((_tmp992[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(
_tmp992,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp993))))),((_tmp98E[1]=Cyc_Absynpp_scope2doc(
_tmp49F),((_tmp98E[0]=tmp_doc,Cyc_PP_cat(_tag_dyneither(_tmp98E,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}return s;};};}struct _tuple17{struct Cyc_Position_Segment*f1;struct
_tuple0*f2;int f3;};struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x);
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x){struct _tuple0*_tmp4B0;
struct _tuple17 _tmp4AF=*x;_tmp4B0=_tmp4AF.f2;return Cyc_Absynpp_qvar2doc(_tmp4B0);}
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d);struct Cyc_PP_Doc*
Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){struct Cyc_PP_Doc*s;{void*_tmp4B1=d->r;
struct Cyc_Absyn_Fndecl*_tmp4B3;struct Cyc_Absyn_Aggrdecl*_tmp4B5;struct Cyc_Absyn_Vardecl*
_tmp4B7;struct Cyc_Absyn_Tvar*_tmp4B9;struct Cyc_Absyn_Vardecl*_tmp4BA;int _tmp4BB;
struct Cyc_Absyn_Exp*_tmp4BC;struct Cyc_Absyn_Exp*_tmp4BE;struct Cyc_Absyn_Tvar*
_tmp4BF;struct Cyc_Absyn_Vardecl*_tmp4C0;struct Cyc_Absyn_Datatypedecl*_tmp4C2;
struct Cyc_Absyn_Datatypedecl _tmp4C3;enum Cyc_Absyn_Scope _tmp4C4;struct _tuple0*
_tmp4C5;struct Cyc_List_List*_tmp4C6;struct Cyc_Core_Opt*_tmp4C7;int _tmp4C8;struct
Cyc_Absyn_Pat*_tmp4CA;struct Cyc_Absyn_Exp*_tmp4CB;struct Cyc_List_List*_tmp4CD;
struct Cyc_Absyn_Enumdecl*_tmp4CF;struct Cyc_Absyn_Enumdecl _tmp4D0;enum Cyc_Absyn_Scope
_tmp4D1;struct _tuple0*_tmp4D2;struct Cyc_Core_Opt*_tmp4D3;struct Cyc_Absyn_Typedefdecl*
_tmp4D5;struct _dyneither_ptr*_tmp4D7;struct Cyc_List_List*_tmp4D8;struct _tuple0*
_tmp4DA;struct Cyc_List_List*_tmp4DB;struct Cyc_List_List*_tmp4DD;struct Cyc_List_List*
_tmp4DF;struct Cyc_List_List*_tmp4E0;_LL274: {struct Cyc_Absyn_Fn_d_struct*_tmp4B2=(
struct Cyc_Absyn_Fn_d_struct*)_tmp4B1;if(_tmp4B2->tag != 1)goto _LL276;else{_tmp4B3=
_tmp4B2->f1;}}_LL275: {struct Cyc_Absyn_FnType_struct _tmp999;struct Cyc_Absyn_FnInfo
_tmp998;struct Cyc_Absyn_FnType_struct*_tmp997;void*t=(void*)((_tmp997=_cycalloc(
sizeof(*_tmp997)),((_tmp997[0]=((_tmp999.tag=10,((_tmp999.f1=((_tmp998.tvars=
_tmp4B3->tvs,((_tmp998.effect=_tmp4B3->effect,((_tmp998.ret_typ=_tmp4B3->ret_type,((
_tmp998.args=((struct Cyc_List_List*(*)(struct _tuple7*(*f)(struct _tuple11*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp4B3->args),((
_tmp998.c_varargs=_tmp4B3->c_varargs,((_tmp998.cyc_varargs=_tmp4B3->cyc_varargs,((
_tmp998.rgn_po=_tmp4B3->rgn_po,((_tmp998.attributes=0,_tmp998)))))))))))))))),
_tmp999)))),_tmp997))));struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4B3->attributes);
struct Cyc_PP_Doc*inlinedoc;if(_tmp4B3->is_inline)switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LL294:{const char*_tmp99A;inlinedoc=Cyc_PP_text(((_tmp99A="inline ",
_tag_dyneither(_tmp99A,sizeof(char),8))));}break;case Cyc_Cyclone_Vc_c: _LL295:{
const char*_tmp99B;inlinedoc=Cyc_PP_text(((_tmp99B="__inline ",_tag_dyneither(
_tmp99B,sizeof(char),10))));}break;}else{inlinedoc=Cyc_PP_nil_doc();}{struct Cyc_PP_Doc*
scopedoc=Cyc_Absynpp_scope2doc(_tmp4B3->sc);struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL297: beforenamedoc=
attsdoc;break;case Cyc_Cyclone_Vc_c: _LL298: beforenamedoc=Cyc_Absynpp_callconv2doc(
_tmp4B3->attributes);break;}{struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(
_tmp4B3->name);struct Cyc_PP_Doc*_tmp99E[2];struct Cyc_Core_Opt*_tmp99D;struct Cyc_PP_Doc*
tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,((_tmp99D=_cycalloc(
sizeof(*_tmp99D)),((_tmp99D->v=((_tmp99E[1]=namedoc,((_tmp99E[0]=beforenamedoc,
Cyc_PP_cat(_tag_dyneither(_tmp99E,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp99D)))));
struct Cyc_PP_Doc*_tmp9A1[2];struct Cyc_PP_Doc*_tmp9A0[5];struct Cyc_PP_Doc*bodydoc=(
_tmp9A0[4]=Cyc_Absynpp_rb(),((_tmp9A0[3]=Cyc_PP_line_doc(),((_tmp9A0[2]=Cyc_PP_nest(
2,((_tmp9A1[1]=Cyc_Absynpp_stmt2doc(_tmp4B3->body,0),((_tmp9A1[0]=Cyc_PP_line_doc(),
Cyc_PP_cat(_tag_dyneither(_tmp9A1,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9A0[1]=
Cyc_Absynpp_lb(),((_tmp9A0[0]=Cyc_PP_blank_doc(),Cyc_PP_cat(_tag_dyneither(
_tmp9A0,sizeof(struct Cyc_PP_Doc*),5)))))))))));{struct Cyc_PP_Doc*_tmp9A2[4];s=((
_tmp9A2[3]=bodydoc,((_tmp9A2[2]=tqtddoc,((_tmp9A2[1]=scopedoc,((_tmp9A2[0]=
inlinedoc,Cyc_PP_cat(_tag_dyneither(_tmp9A2,sizeof(struct Cyc_PP_Doc*),4))))))))));}
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL29A:{struct Cyc_PP_Doc*
_tmp9A3[2];s=((_tmp9A3[1]=s,((_tmp9A3[0]=attsdoc,Cyc_PP_cat(_tag_dyneither(
_tmp9A3,sizeof(struct Cyc_PP_Doc*),2))))));}break;default: _LL29B: break;}goto
_LL273;};};}_LL276: {struct Cyc_Absyn_Aggr_d_struct*_tmp4B4=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp4B1;if(_tmp4B4->tag != 6)goto _LL278;else{_tmp4B5=_tmp4B4->f1;}}_LL277: if(
_tmp4B5->impl == 0){const char*_tmp9A6;struct Cyc_PP_Doc*_tmp9A5[5];s=((_tmp9A5[4]=
Cyc_PP_text(((_tmp9A6=";",_tag_dyneither(_tmp9A6,sizeof(char),2)))),((_tmp9A5[3]=
Cyc_Absynpp_ktvars2doc(_tmp4B5->tvs),((_tmp9A5[2]=Cyc_Absynpp_qvar2bolddoc(
_tmp4B5->name),((_tmp9A5[1]=Cyc_Absynpp_aggr_kind2doc(_tmp4B5->kind),((_tmp9A5[0]=
Cyc_Absynpp_scope2doc(_tmp4B5->sc),Cyc_PP_cat(_tag_dyneither(_tmp9A5,sizeof(
struct Cyc_PP_Doc*),5))))))))))));}else{struct Cyc_PP_Doc*_tmp9B0[2];const char*
_tmp9AF;struct Cyc_PP_Doc*_tmp9AE[2];const char*_tmp9AD;struct Cyc_PP_Doc*_tmp9AC[
13];s=((_tmp9AC[12]=Cyc_PP_text(((_tmp9AD=";",_tag_dyneither(_tmp9AD,sizeof(char),
2)))),((_tmp9AC[11]=Cyc_Absynpp_atts2doc(_tmp4B5->attributes),((_tmp9AC[10]=Cyc_Absynpp_rb(),((
_tmp9AC[9]=Cyc_PP_line_doc(),((_tmp9AC[8]=Cyc_PP_nest(2,((_tmp9AE[1]=Cyc_Absynpp_aggrfields2doc(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4B5->impl))->fields),((_tmp9AE[0]=
Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9AE,sizeof(struct Cyc_PP_Doc*),2))))))),((
_tmp9AC[7]=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4B5->impl))->rgn_po == 
0?Cyc_PP_nil_doc():((_tmp9B0[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4B5->impl))->rgn_po),((_tmp9B0[0]=Cyc_PP_text(((_tmp9AF=":",
_tag_dyneither(_tmp9AF,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp9B0,
sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp9AC[6]=Cyc_Absynpp_ktvars2doc(((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4B5->impl))->exist_vars),((_tmp9AC[5]=
Cyc_Absynpp_lb(),((_tmp9AC[4]=Cyc_PP_blank_doc(),((_tmp9AC[3]=Cyc_Absynpp_ktvars2doc(
_tmp4B5->tvs),((_tmp9AC[2]=Cyc_Absynpp_qvar2bolddoc(_tmp4B5->name),((_tmp9AC[1]=
Cyc_Absynpp_aggr_kind2doc(_tmp4B5->kind),((_tmp9AC[0]=Cyc_Absynpp_scope2doc(
_tmp4B5->sc),Cyc_PP_cat(_tag_dyneither(_tmp9AC,sizeof(struct Cyc_PP_Doc*),13))))))))))))))))))))))))))));}
goto _LL273;_LL278: {struct Cyc_Absyn_Var_d_struct*_tmp4B6=(struct Cyc_Absyn_Var_d_struct*)
_tmp4B1;if(_tmp4B6->tag != 0)goto _LL27A;else{_tmp4B7=_tmp4B6->f1;}}_LL279: s=Cyc_Absynpp_vardecl2doc(
_tmp4B7);goto _LL273;_LL27A: {struct Cyc_Absyn_Region_d_struct*_tmp4B8=(struct Cyc_Absyn_Region_d_struct*)
_tmp4B1;if(_tmp4B8->tag != 4)goto _LL27C;else{_tmp4B9=_tmp4B8->f1;_tmp4BA=_tmp4B8->f2;
_tmp4BB=_tmp4B8->f3;_tmp4BC=_tmp4B8->f4;}}_LL27B:{const char*_tmp9C3;const char*
_tmp9C2;const char*_tmp9C1;const char*_tmp9C0;struct Cyc_PP_Doc*_tmp9BF[3];const
char*_tmp9BE;const char*_tmp9BD;const char*_tmp9BC;struct Cyc_PP_Doc*_tmp9BB[8];s=((
_tmp9BB[7]=Cyc_PP_text(((_tmp9BC=";",_tag_dyneither(_tmp9BC,sizeof(char),2)))),((
_tmp9BB[6]=_tmp4BC != 0?(_tmp9BF[2]=Cyc_PP_text(((_tmp9BE=")",_tag_dyneither(
_tmp9BE,sizeof(char),2)))),((_tmp9BF[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_tmp4BC),((_tmp9BF[0]=Cyc_PP_text(((_tmp9BD=" = open(",_tag_dyneither(_tmp9BD,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp9BF,sizeof(struct Cyc_PP_Doc*),3))))))):
Cyc_PP_nil_doc(),((_tmp9BB[5]=_tmp4BB?Cyc_PP_text(((_tmp9C0=" @resetable",
_tag_dyneither(_tmp9C0,sizeof(char),12)))): Cyc_PP_nil_doc(),((_tmp9BB[4]=Cyc_Absynpp_qvar2doc(
_tmp4BA->name),((_tmp9BB[3]=Cyc_PP_text(((_tmp9C1=">",_tag_dyneither(_tmp9C1,
sizeof(char),2)))),((_tmp9BB[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp4B9)),((
_tmp9BB[1]=Cyc_PP_text(((_tmp9C2="<",_tag_dyneither(_tmp9C2,sizeof(char),2)))),((
_tmp9BB[0]=Cyc_PP_text(((_tmp9C3="region",_tag_dyneither(_tmp9C3,sizeof(char),7)))),
Cyc_PP_cat(_tag_dyneither(_tmp9BB,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL273;_LL27C: {struct Cyc_Absyn_Alias_d_struct*_tmp4BD=(struct Cyc_Absyn_Alias_d_struct*)
_tmp4B1;if(_tmp4BD->tag != 5)goto _LL27E;else{_tmp4BE=_tmp4BD->f1;_tmp4BF=_tmp4BD->f2;
_tmp4C0=_tmp4BD->f3;}}_LL27D:{const char*_tmp9CE;const char*_tmp9CD;const char*
_tmp9CC;const char*_tmp9CB;const char*_tmp9CA;struct Cyc_PP_Doc*_tmp9C9[8];s=((
_tmp9C9[7]=Cyc_PP_text(((_tmp9CA=";",_tag_dyneither(_tmp9CA,sizeof(char),2)))),((
_tmp9C9[6]=Cyc_Absynpp_exp2doc(_tmp4BE),((_tmp9C9[5]=Cyc_PP_text(((_tmp9CB=" = ",
_tag_dyneither(_tmp9CB,sizeof(char),4)))),((_tmp9C9[4]=Cyc_Absynpp_qvar2doc(
_tmp4C0->name),((_tmp9C9[3]=Cyc_PP_text(((_tmp9CC="> ",_tag_dyneither(_tmp9CC,
sizeof(char),3)))),((_tmp9C9[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp4BF)),((
_tmp9C9[1]=Cyc_PP_text(((_tmp9CD=" <",_tag_dyneither(_tmp9CD,sizeof(char),3)))),((
_tmp9C9[0]=Cyc_PP_text(((_tmp9CE="alias",_tag_dyneither(_tmp9CE,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmp9C9,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL273;_LL27E: {struct Cyc_Absyn_Datatype_d_struct*_tmp4C1=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp4B1;if(_tmp4C1->tag != 7)goto _LL280;else{_tmp4C2=_tmp4C1->f1;_tmp4C3=*_tmp4C2;
_tmp4C4=_tmp4C3.sc;_tmp4C5=_tmp4C3.name;_tmp4C6=_tmp4C3.tvs;_tmp4C7=_tmp4C3.fields;
_tmp4C8=_tmp4C3.is_extensible;}}_LL27F: if(_tmp4C7 == 0){const char*_tmp9D5;const
char*_tmp9D4;const char*_tmp9D3;struct Cyc_PP_Doc*_tmp9D2[6];s=((_tmp9D2[5]=Cyc_PP_text(((
_tmp9D3=";",_tag_dyneither(_tmp9D3,sizeof(char),2)))),((_tmp9D2[4]=Cyc_Absynpp_ktvars2doc(
_tmp4C6),((_tmp9D2[3]=_tmp4C8?Cyc_Absynpp_qvar2bolddoc(_tmp4C5): Cyc_Absynpp_typedef_name2bolddoc(
_tmp4C5),((_tmp9D2[2]=Cyc_PP_text(((_tmp9D4="datatype ",_tag_dyneither(_tmp9D4,
sizeof(char),10)))),((_tmp9D2[1]=_tmp4C8?Cyc_PP_text(((_tmp9D5="@extensible",
_tag_dyneither(_tmp9D5,sizeof(char),12)))): Cyc_PP_blank_doc(),((_tmp9D2[0]=Cyc_Absynpp_scope2doc(
_tmp4C4),Cyc_PP_cat(_tag_dyneither(_tmp9D2,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
else{const char*_tmp9DE;const char*_tmp9DD;struct Cyc_PP_Doc*_tmp9DC[2];const char*
_tmp9DB;struct Cyc_PP_Doc*_tmp9DA[11];s=((_tmp9DA[10]=Cyc_PP_text(((_tmp9DB=";",
_tag_dyneither(_tmp9DB,sizeof(char),2)))),((_tmp9DA[9]=Cyc_Absynpp_rb(),((
_tmp9DA[8]=Cyc_PP_line_doc(),((_tmp9DA[7]=Cyc_PP_nest(2,((_tmp9DC[1]=Cyc_Absynpp_datatypefields2doc((
struct Cyc_List_List*)_tmp4C7->v),((_tmp9DC[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp9DC,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9DA[6]=Cyc_Absynpp_lb(),((
_tmp9DA[5]=Cyc_PP_blank_doc(),((_tmp9DA[4]=Cyc_Absynpp_ktvars2doc(_tmp4C6),((
_tmp9DA[3]=_tmp4C8?Cyc_Absynpp_qvar2bolddoc(_tmp4C5): Cyc_Absynpp_typedef_name2bolddoc(
_tmp4C5),((_tmp9DA[2]=Cyc_PP_text(((_tmp9DD="datatype ",_tag_dyneither(_tmp9DD,
sizeof(char),10)))),((_tmp9DA[1]=_tmp4C8?Cyc_PP_text(((_tmp9DE="@extensible ",
_tag_dyneither(_tmp9DE,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp9DA[0]=Cyc_Absynpp_scope2doc(
_tmp4C4),Cyc_PP_cat(_tag_dyneither(_tmp9DA,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL273;_LL280: {struct Cyc_Absyn_Let_d_struct*_tmp4C9=(struct Cyc_Absyn_Let_d_struct*)
_tmp4B1;if(_tmp4C9->tag != 2)goto _LL282;else{_tmp4CA=_tmp4C9->f1;_tmp4CB=_tmp4C9->f3;}}
_LL281:{const char*_tmp9E5;const char*_tmp9E4;const char*_tmp9E3;struct Cyc_PP_Doc*
_tmp9E2[5];s=((_tmp9E2[4]=Cyc_PP_text(((_tmp9E3=";",_tag_dyneither(_tmp9E3,
sizeof(char),2)))),((_tmp9E2[3]=Cyc_Absynpp_exp2doc(_tmp4CB),((_tmp9E2[2]=Cyc_PP_text(((
_tmp9E4=" = ",_tag_dyneither(_tmp9E4,sizeof(char),4)))),((_tmp9E2[1]=Cyc_Absynpp_pat2doc(
_tmp4CA),((_tmp9E2[0]=Cyc_PP_text(((_tmp9E5="let ",_tag_dyneither(_tmp9E5,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp9E2,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL273;_LL282: {struct Cyc_Absyn_Letv_d_struct*_tmp4CC=(struct Cyc_Absyn_Letv_d_struct*)
_tmp4B1;if(_tmp4CC->tag != 3)goto _LL284;else{_tmp4CD=_tmp4CC->f1;}}_LL283:{const
char*_tmp9EA;const char*_tmp9E9;struct Cyc_PP_Doc*_tmp9E8[3];s=((_tmp9E8[2]=Cyc_PP_text(((
_tmp9E9=";",_tag_dyneither(_tmp9E9,sizeof(char),2)))),((_tmp9E8[1]=Cyc_Absynpp_ids2doc(
_tmp4CD),((_tmp9E8[0]=Cyc_PP_text(((_tmp9EA="let ",_tag_dyneither(_tmp9EA,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp9E8,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL273;_LL284: {struct Cyc_Absyn_Enum_d_struct*_tmp4CE=(struct Cyc_Absyn_Enum_d_struct*)
_tmp4B1;if(_tmp4CE->tag != 8)goto _LL286;else{_tmp4CF=_tmp4CE->f1;_tmp4D0=*_tmp4CF;
_tmp4D1=_tmp4D0.sc;_tmp4D2=_tmp4D0.name;_tmp4D3=_tmp4D0.fields;}}_LL285: if(
_tmp4D3 == 0){const char*_tmp9EF;const char*_tmp9EE;struct Cyc_PP_Doc*_tmp9ED[4];s=((
_tmp9ED[3]=Cyc_PP_text(((_tmp9EE=";",_tag_dyneither(_tmp9EE,sizeof(char),2)))),((
_tmp9ED[2]=Cyc_Absynpp_typedef_name2bolddoc(_tmp4D2),((_tmp9ED[1]=Cyc_PP_text(((
_tmp9EF="enum ",_tag_dyneither(_tmp9EF,sizeof(char),6)))),((_tmp9ED[0]=Cyc_Absynpp_scope2doc(
_tmp4D1),Cyc_PP_cat(_tag_dyneither(_tmp9ED,sizeof(struct Cyc_PP_Doc*),4))))))))));}
else{const char*_tmp9F6;struct Cyc_PP_Doc*_tmp9F5[2];const char*_tmp9F4;struct Cyc_PP_Doc*
_tmp9F3[9];s=((_tmp9F3[8]=Cyc_PP_text(((_tmp9F4=";",_tag_dyneither(_tmp9F4,
sizeof(char),2)))),((_tmp9F3[7]=Cyc_Absynpp_rb(),((_tmp9F3[6]=Cyc_PP_line_doc(),((
_tmp9F3[5]=Cyc_PP_nest(2,((_tmp9F5[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)
_tmp4D3->v),((_tmp9F5[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9F5,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9F3[4]=Cyc_Absynpp_lb(),((_tmp9F3[3]=
Cyc_PP_blank_doc(),((_tmp9F3[2]=Cyc_Absynpp_qvar2bolddoc(_tmp4D2),((_tmp9F3[1]=
Cyc_PP_text(((_tmp9F6="enum ",_tag_dyneither(_tmp9F6,sizeof(char),6)))),((
_tmp9F3[0]=Cyc_Absynpp_scope2doc(_tmp4D1),Cyc_PP_cat(_tag_dyneither(_tmp9F3,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL273;_LL286: {struct Cyc_Absyn_Typedef_d_struct*
_tmp4D4=(struct Cyc_Absyn_Typedef_d_struct*)_tmp4B1;if(_tmp4D4->tag != 9)goto
_LL288;else{_tmp4D5=_tmp4D4->f1;}}_LL287: {void*t;if(_tmp4D5->defn != 0)t=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4D5->defn))->v;else{t=Cyc_Absyn_new_evar(
_tmp4D5->kind,0);}{const char*_tmpA00;struct Cyc_Core_Opt*_tmp9FF;struct Cyc_PP_Doc*
_tmp9FE[2];const char*_tmp9FD;struct Cyc_PP_Doc*_tmp9FC[4];s=((_tmp9FC[3]=Cyc_PP_text(((
_tmp9FD=";",_tag_dyneither(_tmp9FD,sizeof(char),2)))),((_tmp9FC[2]=Cyc_Absynpp_atts2doc(
_tmp4D5->atts),((_tmp9FC[1]=Cyc_Absynpp_tqtd2doc(_tmp4D5->tq,t,((_tmp9FF=
_cycalloc(sizeof(*_tmp9FF)),((_tmp9FF->v=((_tmp9FE[1]=Cyc_Absynpp_tvars2doc(
_tmp4D5->tvs),((_tmp9FE[0]=Cyc_Absynpp_typedef_name2bolddoc(_tmp4D5->name),Cyc_PP_cat(
_tag_dyneither(_tmp9FE,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9FF))))),((_tmp9FC[
0]=Cyc_PP_text(((_tmpA00="typedef ",_tag_dyneither(_tmpA00,sizeof(char),9)))),
Cyc_PP_cat(_tag_dyneither(_tmp9FC,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto
_LL273;}_LL288: {struct Cyc_Absyn_Namespace_d_struct*_tmp4D6=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp4B1;if(_tmp4D6->tag != 10)goto _LL28A;else{_tmp4D7=_tmp4D6->f1;_tmp4D8=_tmp4D6->f2;}}
_LL289: if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp4D7);{
const char*_tmpA05;const char*_tmpA04;struct Cyc_PP_Doc*_tmpA03[8];s=((_tmpA03[7]=
Cyc_Absynpp_rb(),((_tmpA03[6]=Cyc_PP_line_doc(),((_tmpA03[5]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA04="",_tag_dyneither(_tmpA04,
sizeof(char),1))),_tmp4D8),((_tmpA03[4]=Cyc_PP_line_doc(),((_tmpA03[3]=Cyc_Absynpp_lb(),((
_tmpA03[2]=Cyc_PP_blank_doc(),((_tmpA03[1]=Cyc_PP_textptr(_tmp4D7),((_tmpA03[0]=
Cyc_PP_text(((_tmpA05="namespace ",_tag_dyneither(_tmpA05,sizeof(char),11)))),
Cyc_PP_cat(_tag_dyneither(_tmpA03,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();goto _LL273;
_LL28A: {struct Cyc_Absyn_Using_d_struct*_tmp4D9=(struct Cyc_Absyn_Using_d_struct*)
_tmp4B1;if(_tmp4D9->tag != 11)goto _LL28C;else{_tmp4DA=_tmp4D9->f1;_tmp4DB=_tmp4D9->f2;}}
_LL28B: if(Cyc_Absynpp_print_using_stmts){const char*_tmpA0A;const char*_tmpA09;
struct Cyc_PP_Doc*_tmpA08[8];s=((_tmpA08[7]=Cyc_Absynpp_rb(),((_tmpA08[6]=Cyc_PP_line_doc(),((
_tmpA08[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA09="",_tag_dyneither(_tmpA09,sizeof(char),1))),_tmp4DB),((_tmpA08[4]=Cyc_PP_line_doc(),((
_tmpA08[3]=Cyc_Absynpp_lb(),((_tmpA08[2]=Cyc_PP_blank_doc(),((_tmpA08[1]=Cyc_Absynpp_qvar2doc(
_tmp4DA),((_tmpA08[0]=Cyc_PP_text(((_tmpA0A="using ",_tag_dyneither(_tmpA0A,
sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmpA08,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
else{const char*_tmpA15;const char*_tmpA14;const char*_tmpA13;const char*_tmpA12;
const char*_tmpA11;struct Cyc_PP_Doc*_tmpA10[11];s=((_tmpA10[10]=Cyc_PP_text(((
_tmpA11=" */",_tag_dyneither(_tmpA11,sizeof(char),4)))),((_tmpA10[9]=Cyc_Absynpp_rb(),((
_tmpA10[8]=Cyc_PP_text(((_tmpA12="/* ",_tag_dyneither(_tmpA12,sizeof(char),4)))),((
_tmpA10[7]=Cyc_PP_line_doc(),((_tmpA10[6]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmpA13="",_tag_dyneither(_tmpA13,sizeof(char),1))),
_tmp4DB),((_tmpA10[5]=Cyc_PP_line_doc(),((_tmpA10[4]=Cyc_PP_text(((_tmpA14=" */",
_tag_dyneither(_tmpA14,sizeof(char),4)))),((_tmpA10[3]=Cyc_Absynpp_lb(),((
_tmpA10[2]=Cyc_PP_blank_doc(),((_tmpA10[1]=Cyc_Absynpp_qvar2doc(_tmp4DA),((
_tmpA10[0]=Cyc_PP_text(((_tmpA15="/* using ",_tag_dyneither(_tmpA15,sizeof(char),
10)))),Cyc_PP_cat(_tag_dyneither(_tmpA10,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL273;_LL28C: {struct Cyc_Absyn_ExternC_d_struct*_tmp4DC=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp4B1;if(_tmp4DC->tag != 12)goto _LL28E;else{_tmp4DD=_tmp4DC->f1;}}_LL28D: if(Cyc_Absynpp_print_externC_stmts){
const char*_tmpA1A;const char*_tmpA19;struct Cyc_PP_Doc*_tmpA18[6];s=((_tmpA18[5]=
Cyc_Absynpp_rb(),((_tmpA18[4]=Cyc_PP_line_doc(),((_tmpA18[3]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA19="",_tag_dyneither(_tmpA19,
sizeof(char),1))),_tmp4DD),((_tmpA18[2]=Cyc_PP_line_doc(),((_tmpA18[1]=Cyc_Absynpp_lb(),((
_tmpA18[0]=Cyc_PP_text(((_tmpA1A="extern \"C\" ",_tag_dyneither(_tmpA1A,sizeof(
char),12)))),Cyc_PP_cat(_tag_dyneither(_tmpA18,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
else{const char*_tmpA25;const char*_tmpA24;const char*_tmpA23;const char*_tmpA22;
const char*_tmpA21;struct Cyc_PP_Doc*_tmpA20[9];s=((_tmpA20[8]=Cyc_PP_text(((
_tmpA21=" */",_tag_dyneither(_tmpA21,sizeof(char),4)))),((_tmpA20[7]=Cyc_Absynpp_rb(),((
_tmpA20[6]=Cyc_PP_text(((_tmpA22="/* ",_tag_dyneither(_tmpA22,sizeof(char),4)))),((
_tmpA20[5]=Cyc_PP_line_doc(),((_tmpA20[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmpA23="",_tag_dyneither(_tmpA23,sizeof(char),1))),
_tmp4DD),((_tmpA20[3]=Cyc_PP_line_doc(),((_tmpA20[2]=Cyc_PP_text(((_tmpA24=" */",
_tag_dyneither(_tmpA24,sizeof(char),4)))),((_tmpA20[1]=Cyc_Absynpp_lb(),((
_tmpA20[0]=Cyc_PP_text(((_tmpA25="/* extern \"C\" ",_tag_dyneither(_tmpA25,
sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmpA20,sizeof(struct Cyc_PP_Doc*),
9))))))))))))))))))));}goto _LL273;_LL28E: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp4DE=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp4B1;if(_tmp4DE->tag != 13)
goto _LL290;else{_tmp4DF=_tmp4DE->f1;_tmp4E0=_tmp4DE->f2;}}_LL28F: if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;if(_tmp4E0 != 0){const char*_tmpA2A;const char*_tmpA29;
struct Cyc_PP_Doc*_tmpA28[7];exs_doc=((_tmpA28[6]=Cyc_Absynpp_rb(),((_tmpA28[5]=
Cyc_PP_line_doc(),((_tmpA28[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_export2doc,((_tmpA29=",",_tag_dyneither(_tmpA29,sizeof(char),2))),
_tmp4E0),((_tmpA28[3]=Cyc_PP_line_doc(),((_tmpA28[2]=Cyc_Absynpp_lb(),((_tmpA28[
1]=Cyc_PP_text(((_tmpA2A=" export ",_tag_dyneither(_tmpA2A,sizeof(char),9)))),((
_tmpA28[0]=Cyc_Absynpp_rb(),Cyc_PP_cat(_tag_dyneither(_tmpA28,sizeof(struct Cyc_PP_Doc*),
7))))))))))))))));}else{exs_doc=Cyc_Absynpp_rb();}{const char*_tmpA2F;const char*
_tmpA2E;struct Cyc_PP_Doc*_tmpA2D[6];s=((_tmpA2D[5]=exs_doc,((_tmpA2D[4]=Cyc_PP_line_doc(),((
_tmpA2D[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA2E="",_tag_dyneither(_tmpA2E,sizeof(char),1))),_tmp4DF),((_tmpA2D[2]=Cyc_PP_line_doc(),((
_tmpA2D[1]=Cyc_Absynpp_lb(),((_tmpA2D[0]=Cyc_PP_text(((_tmpA2F="extern \"C include\" ",
_tag_dyneither(_tmpA2F,sizeof(char),20)))),Cyc_PP_cat(_tag_dyneither(_tmpA2D,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));};}else{const char*_tmpA3A;const char*
_tmpA39;const char*_tmpA38;const char*_tmpA37;const char*_tmpA36;struct Cyc_PP_Doc*
_tmpA35[9];s=((_tmpA35[8]=Cyc_PP_text(((_tmpA36=" */",_tag_dyneither(_tmpA36,
sizeof(char),4)))),((_tmpA35[7]=Cyc_Absynpp_rb(),((_tmpA35[6]=Cyc_PP_text(((
_tmpA37="/* ",_tag_dyneither(_tmpA37,sizeof(char),4)))),((_tmpA35[5]=Cyc_PP_line_doc(),((
_tmpA35[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA38="",_tag_dyneither(_tmpA38,sizeof(char),1))),_tmp4DF),((_tmpA35[3]=Cyc_PP_line_doc(),((
_tmpA35[2]=Cyc_PP_text(((_tmpA39=" */",_tag_dyneither(_tmpA39,sizeof(char),4)))),((
_tmpA35[1]=Cyc_Absynpp_lb(),((_tmpA35[0]=Cyc_PP_text(((_tmpA3A="/* extern \"C include\" ",
_tag_dyneither(_tmpA3A,sizeof(char),23)))),Cyc_PP_cat(_tag_dyneither(_tmpA35,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL273;_LL290: {struct Cyc_Absyn_Porton_d_struct*
_tmp4E1=(struct Cyc_Absyn_Porton_d_struct*)_tmp4B1;if(_tmp4E1->tag != 14)goto
_LL292;}_LL291:{const char*_tmpA3D;struct Cyc_PP_Doc*_tmpA3C[2];s=((_tmpA3C[1]=Cyc_Absynpp_lb(),((
_tmpA3C[0]=Cyc_PP_text(((_tmpA3D="__cyclone_port_on__;",_tag_dyneither(_tmpA3D,
sizeof(char),21)))),Cyc_PP_cat(_tag_dyneither(_tmpA3C,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LL273;_LL292: {struct Cyc_Absyn_Portoff_d_struct*_tmp4E2=(struct Cyc_Absyn_Portoff_d_struct*)
_tmp4B1;if(_tmp4E2->tag != 15)goto _LL273;}_LL293:{const char*_tmpA40;struct Cyc_PP_Doc*
_tmpA3F[2];s=((_tmpA3F[1]=Cyc_Absynpp_lb(),((_tmpA3F[0]=Cyc_PP_text(((_tmpA40="__cyclone_port_off__;",
_tag_dyneither(_tmpA40,sizeof(char),22)))),Cyc_PP_cat(_tag_dyneither(_tmpA3F,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL273;_LL273:;}return s;}struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc);struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(
enum Cyc_Absyn_Scope sc){if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();
switch(sc){case Cyc_Absyn_Static: _LL29D: {const char*_tmpA41;return Cyc_PP_text(((
_tmpA41="static ",_tag_dyneither(_tmpA41,sizeof(char),8))));}case Cyc_Absyn_Public:
_LL29E: return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL29F: {const char*_tmpA42;
return Cyc_PP_text(((_tmpA42="extern ",_tag_dyneither(_tmpA42,sizeof(char),8))));}
case Cyc_Absyn_ExternC: _LL2A0: {const char*_tmpA43;return Cyc_PP_text(((_tmpA43="extern \"C\" ",
_tag_dyneither(_tmpA43,sizeof(char),12))));}case Cyc_Absyn_Abstract: _LL2A1: {
const char*_tmpA44;return Cyc_PP_text(((_tmpA44="abstract ",_tag_dyneither(_tmpA44,
sizeof(char),10))));}case Cyc_Absyn_Register: _LL2A2: {const char*_tmpA45;return Cyc_PP_text(((
_tmpA45="register ",_tag_dyneither(_tmpA45,sizeof(char),10))));}}}int Cyc_Absynpp_exists_temp_tvar_in_effect(
void*t);int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){void*_tmp54A=t;struct
Cyc_Absyn_Tvar*_tmp54C;struct Cyc_List_List*_tmp54E;_LL2A5: {struct Cyc_Absyn_VarType_struct*
_tmp54B=(struct Cyc_Absyn_VarType_struct*)_tmp54A;if(_tmp54B->tag != 2)goto _LL2A7;
else{_tmp54C=_tmp54B->f1;}}_LL2A6: return Cyc_Tcutil_is_temp_tvar(_tmp54C);_LL2A7: {
struct Cyc_Absyn_JoinEff_struct*_tmp54D=(struct Cyc_Absyn_JoinEff_struct*)_tmp54A;
if(_tmp54D->tag != 24)goto _LL2A9;else{_tmp54E=_tmp54D->f1;}}_LL2A8: return((int(*)(
int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,
_tmp54E);_LL2A9:;_LL2AA: return 0;_LL2A4:;}int Cyc_Absynpp_is_anon_aggrtype(void*t);
int Cyc_Absynpp_is_anon_aggrtype(void*t){void*_tmp54F=t;void**_tmp553;void*
_tmp554;_LL2AC: {struct Cyc_Absyn_AnonAggrType_struct*_tmp550=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp54F;if(_tmp550->tag != 13)goto _LL2AE;}_LL2AD: return 1;_LL2AE: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp551=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp54F;if(_tmp551->tag != 15)goto
_LL2B0;}_LL2AF: return 1;_LL2B0: {struct Cyc_Absyn_TypedefType_struct*_tmp552=(
struct Cyc_Absyn_TypedefType_struct*)_tmp54F;if(_tmp552->tag != 18)goto _LL2B2;
else{_tmp553=_tmp552->f4;if(_tmp553 == 0)goto _LL2B2;_tmp554=*_tmp553;}}_LL2B1:
return Cyc_Absynpp_is_anon_aggrtype(_tmp554);_LL2B2:;_LL2B3: return 0;_LL2AB:;}
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,
void*atts,struct Cyc_List_List*tms);static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(
struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){if(tms != 0  && tms->tl != 
0){struct _tuple16 _tmpA46;struct _tuple16 _tmp556=(_tmpA46.f1=(void*)tms->hd,((
_tmpA46.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,_tmpA46)));
void*_tmp557;void*_tmp559;_LL2B5: _tmp557=_tmp556.f1;{struct Cyc_Absyn_Pointer_mod_struct*
_tmp558=(struct Cyc_Absyn_Pointer_mod_struct*)_tmp557;if(_tmp558->tag != 2)goto
_LL2B7;};_tmp559=_tmp556.f2;{struct Cyc_Absyn_Function_mod_struct*_tmp55A=(struct
Cyc_Absyn_Function_mod_struct*)_tmp559;if(_tmp55A->tag != 3)goto _LL2B7;};_LL2B6: {
struct Cyc_List_List*_tmpA49;struct Cyc_List_List*_tmpA48;return(_tmpA48=
_region_malloc(r,sizeof(*_tmpA48)),((_tmpA48->hd=(void*)tms->hd,((_tmpA48->tl=((
_tmpA49=_region_malloc(r,sizeof(*_tmpA49)),((_tmpA49->hd=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd,((_tmpA49->tl=Cyc_Absynpp_bubble_attributes(r,atts,((
struct Cyc_List_List*)_check_null(tms->tl))->tl),_tmpA49)))))),_tmpA48)))));}
_LL2B7:;_LL2B8: {struct Cyc_List_List*_tmpA4A;return(_tmpA4A=_region_malloc(r,
sizeof(*_tmpA4A)),((_tmpA4A->hd=atts,((_tmpA4A->tl=tms,_tmpA4A)))));}_LL2B4:;}
else{struct Cyc_List_List*_tmpA4B;return(_tmpA4B=_region_malloc(r,sizeof(*_tmpA4B)),((
_tmpA4B->hd=atts,((_tmpA4B->tl=tms,_tmpA4B)))));}}struct _tuple12 Cyc_Absynpp_to_tms(
struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t);struct _tuple12 Cyc_Absynpp_to_tms(
struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){void*_tmp55F=t;struct Cyc_Absyn_ArrayInfo
_tmp561;void*_tmp562;struct Cyc_Absyn_Tqual _tmp563;struct Cyc_Absyn_Exp*_tmp564;
union Cyc_Absyn_Constraint*_tmp565;struct Cyc_Position_Segment*_tmp566;struct Cyc_Absyn_PtrInfo
_tmp568;void*_tmp569;struct Cyc_Absyn_Tqual _tmp56A;struct Cyc_Absyn_PtrAtts _tmp56B;
struct Cyc_Absyn_FnInfo _tmp56D;struct Cyc_List_List*_tmp56E;struct Cyc_Core_Opt*
_tmp56F;void*_tmp570;struct Cyc_List_List*_tmp571;int _tmp572;struct Cyc_Absyn_VarargInfo*
_tmp573;struct Cyc_List_List*_tmp574;struct Cyc_List_List*_tmp575;struct Cyc_Core_Opt*
_tmp577;struct Cyc_Core_Opt*_tmp578;int _tmp579;struct _tuple0*_tmp57B;struct Cyc_List_List*
_tmp57C;void**_tmp57D;_LL2BA: {struct Cyc_Absyn_ArrayType_struct*_tmp560=(struct
Cyc_Absyn_ArrayType_struct*)_tmp55F;if(_tmp560->tag != 9)goto _LL2BC;else{_tmp561=
_tmp560->f1;_tmp562=_tmp561.elt_type;_tmp563=_tmp561.tq;_tmp564=_tmp561.num_elts;
_tmp565=_tmp561.zero_term;_tmp566=_tmp561.zt_loc;}}_LL2BB: {struct Cyc_Absyn_Tqual
_tmp57F;void*_tmp580;struct Cyc_List_List*_tmp581;struct _tuple12 _tmp57E=Cyc_Absynpp_to_tms(
r,_tmp563,_tmp562);_tmp57F=_tmp57E.f1;_tmp580=_tmp57E.f2;_tmp581=_tmp57E.f3;{
void*tm;if(_tmp564 == 0){struct Cyc_Absyn_Carray_mod_struct _tmpA4E;struct Cyc_Absyn_Carray_mod_struct*
_tmpA4D;tm=(void*)((_tmpA4D=_region_malloc(r,sizeof(*_tmpA4D)),((_tmpA4D[0]=((
_tmpA4E.tag=0,((_tmpA4E.f1=_tmp565,((_tmpA4E.f2=_tmp566,_tmpA4E)))))),_tmpA4D))));}
else{struct Cyc_Absyn_ConstArray_mod_struct _tmpA51;struct Cyc_Absyn_ConstArray_mod_struct*
_tmpA50;tm=(void*)((_tmpA50=_region_malloc(r,sizeof(*_tmpA50)),((_tmpA50[0]=((
_tmpA51.tag=1,((_tmpA51.f1=(struct Cyc_Absyn_Exp*)_tmp564,((_tmpA51.f2=_tmp565,((
_tmpA51.f3=_tmp566,_tmpA51)))))))),_tmpA50))));}{struct Cyc_List_List*_tmpA54;
struct _tuple12 _tmpA53;return(_tmpA53.f1=_tmp57F,((_tmpA53.f2=_tmp580,((_tmpA53.f3=((
_tmpA54=_region_malloc(r,sizeof(*_tmpA54)),((_tmpA54->hd=tm,((_tmpA54->tl=
_tmp581,_tmpA54)))))),_tmpA53)))));};};}_LL2BC: {struct Cyc_Absyn_PointerType_struct*
_tmp567=(struct Cyc_Absyn_PointerType_struct*)_tmp55F;if(_tmp567->tag != 5)goto
_LL2BE;else{_tmp568=_tmp567->f1;_tmp569=_tmp568.elt_typ;_tmp56A=_tmp568.elt_tq;
_tmp56B=_tmp568.ptr_atts;}}_LL2BD: {struct Cyc_Absyn_Tqual _tmp589;void*_tmp58A;
struct Cyc_List_List*_tmp58B;struct _tuple12 _tmp588=Cyc_Absynpp_to_tms(r,_tmp56A,
_tmp569);_tmp589=_tmp588.f1;_tmp58A=_tmp588.f2;_tmp58B=_tmp588.f3;{struct Cyc_Absyn_Pointer_mod_struct*
_tmpA5A;struct Cyc_Absyn_Pointer_mod_struct _tmpA59;struct Cyc_List_List*_tmpA58;
_tmp58B=((_tmpA58=_region_malloc(r,sizeof(*_tmpA58)),((_tmpA58->hd=(void*)((
_tmpA5A=_region_malloc(r,sizeof(*_tmpA5A)),((_tmpA5A[0]=((_tmpA59.tag=2,((
_tmpA59.f1=_tmp56B,((_tmpA59.f2=tq,_tmpA59)))))),_tmpA5A)))),((_tmpA58->tl=
_tmp58B,_tmpA58))))));}{struct _tuple12 _tmpA5B;return(_tmpA5B.f1=_tmp589,((
_tmpA5B.f2=_tmp58A,((_tmpA5B.f3=_tmp58B,_tmpA5B)))));};}_LL2BE: {struct Cyc_Absyn_FnType_struct*
_tmp56C=(struct Cyc_Absyn_FnType_struct*)_tmp55F;if(_tmp56C->tag != 10)goto _LL2C0;
else{_tmp56D=_tmp56C->f1;_tmp56E=_tmp56D.tvars;_tmp56F=_tmp56D.effect;_tmp570=
_tmp56D.ret_typ;_tmp571=_tmp56D.args;_tmp572=_tmp56D.c_varargs;_tmp573=_tmp56D.cyc_varargs;
_tmp574=_tmp56D.rgn_po;_tmp575=_tmp56D.attributes;}}_LL2BF: if(!Cyc_Absynpp_print_all_tvars){
if(_tmp56F == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect((void*)_tmp56F->v)){
_tmp56F=0;_tmp56E=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,
_tmp56E);}{struct Cyc_Absyn_Tqual _tmp591;void*_tmp592;struct Cyc_List_List*_tmp593;
struct _tuple12 _tmp590=Cyc_Absynpp_to_tms(r,Cyc_Absyn_empty_tqual(0),_tmp570);
_tmp591=_tmp590.f1;_tmp592=_tmp590.f2;_tmp593=_tmp590.f3;{struct Cyc_List_List*
tms=_tmp593;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2C6: if(
_tmp575 != 0){struct Cyc_Absyn_Attributes_mod_struct _tmpA5E;struct Cyc_Absyn_Attributes_mod_struct*
_tmpA5D;tms=Cyc_Absynpp_bubble_attributes(r,(void*)((_tmpA5D=_region_malloc(r,
sizeof(*_tmpA5D)),((_tmpA5D[0]=((_tmpA5E.tag=5,((_tmpA5E.f1=0,((_tmpA5E.f2=
_tmp575,_tmpA5E)))))),_tmpA5D)))),tms);}{struct Cyc_Absyn_Function_mod_struct*
_tmpA6D;struct Cyc_Absyn_WithTypes_struct*_tmpA6C;struct Cyc_Absyn_WithTypes_struct
_tmpA6B;struct Cyc_Absyn_Function_mod_struct _tmpA6A;struct Cyc_List_List*_tmpA69;
tms=((_tmpA69=_region_malloc(r,sizeof(*_tmpA69)),((_tmpA69->hd=(void*)((_tmpA6D=
_region_malloc(r,sizeof(*_tmpA6D)),((_tmpA6D[0]=((_tmpA6A.tag=3,((_tmpA6A.f1=(
void*)((void*)((_tmpA6C=_region_malloc(r,sizeof(*_tmpA6C)),((_tmpA6C[0]=((
_tmpA6B.tag=1,((_tmpA6B.f1=_tmp571,((_tmpA6B.f2=_tmp572,((_tmpA6B.f3=_tmp573,((
_tmpA6B.f4=_tmp56F,((_tmpA6B.f5=_tmp574,_tmpA6B)))))))))))),_tmpA6C))))),_tmpA6A)))),
_tmpA6D)))),((_tmpA69->tl=tms,_tmpA69))))));}break;case Cyc_Cyclone_Vc_c: _LL2C7:{
struct Cyc_Absyn_Function_mod_struct*_tmpA7C;struct Cyc_Absyn_WithTypes_struct*
_tmpA7B;struct Cyc_Absyn_WithTypes_struct _tmpA7A;struct Cyc_Absyn_Function_mod_struct
_tmpA79;struct Cyc_List_List*_tmpA78;tms=((_tmpA78=_region_malloc(r,sizeof(*
_tmpA78)),((_tmpA78->hd=(void*)((_tmpA7C=_region_malloc(r,sizeof(*_tmpA7C)),((
_tmpA7C[0]=((_tmpA79.tag=3,((_tmpA79.f1=(void*)((void*)((_tmpA7B=_region_malloc(
r,sizeof(*_tmpA7B)),((_tmpA7B[0]=((_tmpA7A.tag=1,((_tmpA7A.f1=_tmp571,((_tmpA7A.f2=
_tmp572,((_tmpA7A.f3=_tmp573,((_tmpA7A.f4=_tmp56F,((_tmpA7A.f5=_tmp574,_tmpA7A)))))))))))),
_tmpA7B))))),_tmpA79)))),_tmpA7C)))),((_tmpA78->tl=tms,_tmpA78))))));}for(0;
_tmp575 != 0;_tmp575=_tmp575->tl){void*_tmp5A0=(void*)_tmp575->hd;_LL2CA: {struct
Cyc_Absyn_Stdcall_att_struct*_tmp5A1=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmp5A0;if(_tmp5A1->tag != 1)goto _LL2CC;}_LL2CB: goto _LL2CD;_LL2CC: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp5A2=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp5A0;if(_tmp5A2->tag != 2)goto
_LL2CE;}_LL2CD: goto _LL2CF;_LL2CE: {struct Cyc_Absyn_Fastcall_att_struct*_tmp5A3=(
struct Cyc_Absyn_Fastcall_att_struct*)_tmp5A0;if(_tmp5A3->tag != 3)goto _LL2D0;}
_LL2CF:{struct Cyc_Absyn_Attributes_mod_struct*_tmpA86;struct Cyc_List_List*
_tmpA85;struct Cyc_Absyn_Attributes_mod_struct _tmpA84;struct Cyc_List_List*_tmpA83;
tms=((_tmpA83=_region_malloc(r,sizeof(*_tmpA83)),((_tmpA83->hd=(void*)((_tmpA86=
_region_malloc(r,sizeof(*_tmpA86)),((_tmpA86[0]=((_tmpA84.tag=5,((_tmpA84.f1=0,((
_tmpA84.f2=((_tmpA85=_cycalloc(sizeof(*_tmpA85)),((_tmpA85->hd=(void*)_tmp575->hd,((
_tmpA85->tl=0,_tmpA85)))))),_tmpA84)))))),_tmpA86)))),((_tmpA83->tl=tms,_tmpA83))))));}
goto AfterAtts;_LL2D0:;_LL2D1: goto _LL2C9;_LL2C9:;}break;}AfterAtts: if(_tmp56E != 0){
struct Cyc_Absyn_TypeParams_mod_struct*_tmpA8C;struct Cyc_Absyn_TypeParams_mod_struct
_tmpA8B;struct Cyc_List_List*_tmpA8A;tms=((_tmpA8A=_region_malloc(r,sizeof(*
_tmpA8A)),((_tmpA8A->hd=(void*)((_tmpA8C=_region_malloc(r,sizeof(*_tmpA8C)),((
_tmpA8C[0]=((_tmpA8B.tag=4,((_tmpA8B.f1=_tmp56E,((_tmpA8B.f2=0,((_tmpA8B.f3=1,
_tmpA8B)))))))),_tmpA8C)))),((_tmpA8A->tl=tms,_tmpA8A))))));}{struct _tuple12
_tmpA8D;return(_tmpA8D.f1=_tmp591,((_tmpA8D.f2=_tmp592,((_tmpA8D.f3=tms,_tmpA8D)))));};};};
_LL2C0: {struct Cyc_Absyn_Evar_struct*_tmp576=(struct Cyc_Absyn_Evar_struct*)
_tmp55F;if(_tmp576->tag != 1)goto _LL2C2;else{_tmp577=_tmp576->f1;_tmp578=_tmp576->f2;
_tmp579=_tmp576->f3;}}_LL2C1: if(_tmp578 == 0){struct _tuple12 _tmpA8E;return(
_tmpA8E.f1=tq,((_tmpA8E.f2=t,((_tmpA8E.f3=0,_tmpA8E)))));}else{return Cyc_Absynpp_to_tms(
r,tq,(void*)_tmp578->v);}_LL2C2: {struct Cyc_Absyn_TypedefType_struct*_tmp57A=(
struct Cyc_Absyn_TypedefType_struct*)_tmp55F;if(_tmp57A->tag != 18)goto _LL2C4;
else{_tmp57B=_tmp57A->f1;_tmp57C=_tmp57A->f2;_tmp57D=_tmp57A->f4;}}_LL2C3: if((
_tmp57D == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype(*
_tmp57D)){struct _tuple12 _tmpA8F;return(_tmpA8F.f1=tq,((_tmpA8F.f2=t,((_tmpA8F.f3=
0,_tmpA8F)))));}else{return Cyc_Absynpp_to_tms(r,tq,*_tmp57D);}_LL2C4:;_LL2C5: {
struct _tuple12 _tmpA90;return(_tmpA90.f1=tq,((_tmpA90.f2=t,((_tmpA90.f3=0,_tmpA90)))));}
_LL2B9:;}static int Cyc_Absynpp_is_char_ptr(void*t);static int Cyc_Absynpp_is_char_ptr(
void*t){void*_tmp5AF=t;struct Cyc_Core_Opt*_tmp5B1;struct Cyc_Core_Opt _tmp5B2;void*
_tmp5B3;struct Cyc_Absyn_PtrInfo _tmp5B5;void*_tmp5B6;_LL2D3: {struct Cyc_Absyn_Evar_struct*
_tmp5B0=(struct Cyc_Absyn_Evar_struct*)_tmp5AF;if(_tmp5B0->tag != 1)goto _LL2D5;
else{_tmp5B1=_tmp5B0->f2;if(_tmp5B1 == 0)goto _LL2D5;_tmp5B2=*_tmp5B1;_tmp5B3=(
void*)_tmp5B2.v;}}_LL2D4: return Cyc_Absynpp_is_char_ptr(_tmp5B3);_LL2D5: {struct
Cyc_Absyn_PointerType_struct*_tmp5B4=(struct Cyc_Absyn_PointerType_struct*)
_tmp5AF;if(_tmp5B4->tag != 5)goto _LL2D7;else{_tmp5B5=_tmp5B4->f1;_tmp5B6=_tmp5B5.elt_typ;}}
_LL2D6: L: {void*_tmp5B7=_tmp5B6;struct Cyc_Core_Opt*_tmp5B9;struct Cyc_Core_Opt
_tmp5BA;void*_tmp5BB;void**_tmp5BD;void*_tmp5BE;enum Cyc_Absyn_Size_of _tmp5C0;
_LL2DA: {struct Cyc_Absyn_Evar_struct*_tmp5B8=(struct Cyc_Absyn_Evar_struct*)
_tmp5B7;if(_tmp5B8->tag != 1)goto _LL2DC;else{_tmp5B9=_tmp5B8->f2;if(_tmp5B9 == 0)
goto _LL2DC;_tmp5BA=*_tmp5B9;_tmp5BB=(void*)_tmp5BA.v;}}_LL2DB: _tmp5B6=_tmp5BB;
goto L;_LL2DC: {struct Cyc_Absyn_TypedefType_struct*_tmp5BC=(struct Cyc_Absyn_TypedefType_struct*)
_tmp5B7;if(_tmp5BC->tag != 18)goto _LL2DE;else{_tmp5BD=_tmp5BC->f4;if(_tmp5BD == 0)
goto _LL2DE;_tmp5BE=*_tmp5BD;}}_LL2DD: _tmp5B6=_tmp5BE;goto L;_LL2DE: {struct Cyc_Absyn_IntType_struct*
_tmp5BF=(struct Cyc_Absyn_IntType_struct*)_tmp5B7;if(_tmp5BF->tag != 6)goto _LL2E0;
else{_tmp5C0=_tmp5BF->f2;if(_tmp5C0 != Cyc_Absyn_Char_sz)goto _LL2E0;}}_LL2DF:
return 1;_LL2E0:;_LL2E1: return 0;_LL2D9:;}_LL2D7:;_LL2D8: return 0;_LL2D2:;}struct
Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*
dopt);struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,
struct Cyc_Core_Opt*dopt){struct _RegionHandle _tmp5C1=_new_region("temp");struct
_RegionHandle*temp=& _tmp5C1;_push_region(temp);{struct Cyc_Absyn_Tqual _tmp5C3;
void*_tmp5C4;struct Cyc_List_List*_tmp5C5;struct _tuple12 _tmp5C2=Cyc_Absynpp_to_tms(
temp,tq,typ);_tmp5C3=_tmp5C2.f1;_tmp5C4=_tmp5C2.f2;_tmp5C5=_tmp5C2.f3;_tmp5C5=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5C5);if(
_tmp5C5 == 0  && dopt == 0){struct Cyc_PP_Doc*_tmpA91[2];struct Cyc_PP_Doc*_tmp5C7=(
_tmpA91[1]=Cyc_Absynpp_ntyp2doc(_tmp5C4),((_tmpA91[0]=Cyc_Absynpp_tqual2doc(
_tmp5C3),Cyc_PP_cat(_tag_dyneither(_tmpA91,sizeof(struct Cyc_PP_Doc*),2)))));
_npop_handler(0);return _tmp5C7;}else{const char*_tmpA94;struct Cyc_PP_Doc*_tmpA93[
4];struct Cyc_PP_Doc*_tmp5CA=(_tmpA93[3]=Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(
typ),dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp5C5),((_tmpA93[2]=
Cyc_PP_text(((_tmpA94=" ",_tag_dyneither(_tmpA94,sizeof(char),2)))),((_tmpA93[1]=
Cyc_Absynpp_ntyp2doc(_tmp5C4),((_tmpA93[0]=Cyc_Absynpp_tqual2doc(_tmp5C3),Cyc_PP_cat(
_tag_dyneither(_tmpA93,sizeof(struct Cyc_PP_Doc*),4)))))))));_npop_handler(0);
return _tmp5CA;}};_pop_region(temp);}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(
struct Cyc_Absyn_Aggrfield*f);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct
Cyc_Absyn_Aggrfield*f){switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c:
_LL2E2: if(f->width != 0){struct Cyc_Core_Opt*_tmpA9B;const char*_tmpA9A;const char*
_tmpA99;struct Cyc_PP_Doc*_tmpA98[5];return(_tmpA98[4]=Cyc_PP_text(((_tmpA99=";",
_tag_dyneither(_tmpA99,sizeof(char),2)))),((_tmpA98[3]=Cyc_Absynpp_atts2doc(f->attributes),((
_tmpA98[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((
_tmpA98[1]=Cyc_PP_text(((_tmpA9A=":",_tag_dyneither(_tmpA9A,sizeof(char),2)))),((
_tmpA98[0]=Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpA9B=_cycalloc(sizeof(*
_tmpA9B)),((_tmpA9B->v=Cyc_PP_textptr(f->name),_tmpA9B))))),Cyc_PP_cat(
_tag_dyneither(_tmpA98,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{struct Cyc_Core_Opt*
_tmpAA0;const char*_tmpA9F;struct Cyc_PP_Doc*_tmpA9E[3];return(_tmpA9E[2]=Cyc_PP_text(((
_tmpA9F=";",_tag_dyneither(_tmpA9F,sizeof(char),2)))),((_tmpA9E[1]=Cyc_Absynpp_atts2doc(
f->attributes),((_tmpA9E[0]=Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAA0=
_cycalloc(sizeof(*_tmpAA0)),((_tmpAA0->v=Cyc_PP_textptr(f->name),_tmpAA0))))),
Cyc_PP_cat(_tag_dyneither(_tmpA9E,sizeof(struct Cyc_PP_Doc*),3)))))));}case Cyc_Cyclone_Vc_c:
_LL2E3: if(f->width != 0){struct Cyc_Core_Opt*_tmpAA7;const char*_tmpAA6;const char*
_tmpAA5;struct Cyc_PP_Doc*_tmpAA4[5];return(_tmpAA4[4]=Cyc_PP_text(((_tmpAA5=";",
_tag_dyneither(_tmpAA5,sizeof(char),2)))),((_tmpAA4[3]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpAA4[2]=Cyc_PP_text(((_tmpAA6=":",
_tag_dyneither(_tmpAA6,sizeof(char),2)))),((_tmpAA4[1]=Cyc_Absynpp_tqtd2doc(f->tq,
f->type,((_tmpAA7=_cycalloc(sizeof(*_tmpAA7)),((_tmpAA7->v=Cyc_PP_textptr(f->name),
_tmpAA7))))),((_tmpAA4[0]=Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(
_tag_dyneither(_tmpAA4,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{struct Cyc_Core_Opt*
_tmpAAC;const char*_tmpAAB;struct Cyc_PP_Doc*_tmpAAA[3];return(_tmpAAA[2]=Cyc_PP_text(((
_tmpAAB=";",_tag_dyneither(_tmpAAB,sizeof(char),2)))),((_tmpAAA[1]=Cyc_Absynpp_tqtd2doc(
f->tq,f->type,((_tmpAAC=_cycalloc(sizeof(*_tmpAAC)),((_tmpAAC->v=Cyc_PP_textptr(
f->name),_tmpAAC))))),((_tmpAAA[0]=Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(
_tag_dyneither(_tmpAAA,sizeof(struct Cyc_PP_Doc*),3)))))));}}}struct Cyc_PP_Doc*
Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(
struct Cyc_List_List*fields){const char*_tmpAAD;return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,
struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,((_tmpAAD="",
_tag_dyneither(_tmpAAD,sizeof(char),1))),fields);}struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(
struct Cyc_Absyn_Datatypefield*f);struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(
struct Cyc_Absyn_Datatypefield*f){struct Cyc_PP_Doc*_tmpAAE[3];return(_tmpAAE[2]=f->typs
== 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs),((_tmpAAE[1]=Cyc_Absynpp_typedef_name2doc(
f->name),((_tmpAAE[0]=Cyc_Absynpp_scope2doc(f->sc),Cyc_PP_cat(_tag_dyneither(
_tmpAAE,sizeof(struct Cyc_PP_Doc*),3)))))));}struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(
struct Cyc_List_List*fields){const char*_tmpAAF;return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _dyneither_ptr sep,
struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,((_tmpAAF=",",
_tag_dyneither(_tmpAAF,sizeof(char),2))),fields);}void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){for(0;tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(
Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);{const char*_tmpAB2;
void*_tmpAB1;(_tmpAB1=0,Cyc_fprintf(f,((_tmpAB2="\n",_tag_dyneither(_tmpAB2,
sizeof(char),2))),_tag_dyneither(_tmpAB1,sizeof(void*),0)));};}}struct
_dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);struct
_dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){const char*
_tmpAB3;return Cyc_PP_string_of_doc(Cyc_PP_seql(((_tmpAB3="",_tag_dyneither(
_tmpAB3,sizeof(char),1))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
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
