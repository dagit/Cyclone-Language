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
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*);struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
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
v){struct Cyc_List_List*_tmp5D8;Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,((
_tmp5D8=_cycalloc(sizeof(*_tmp5D8)),((_tmp5D8->hd=v,((_tmp5D8->tl=0,_tmp5D8)))))));}
static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l);static void Cyc_Absynpp_suppr_last(
struct Cyc_List_List**l){if(((struct Cyc_List_List*)_check_null(*l))->tl == 0)*l=0;
else{Cyc_Absynpp_suppr_last(&((struct Cyc_List_List*)_check_null(*l))->tl);}}
static void Cyc_Absynpp_curr_namespace_drop();static void Cyc_Absynpp_curr_namespace_drop(){((
void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
struct _dyneither_ptr Cyc_Absynpp_char_escape(char c);struct _dyneither_ptr Cyc_Absynpp_char_escape(
char c){switch(c){case '\a': _LL0: {const char*_tmp5D9;return(_tmp5D9="\\a",
_tag_dyneither(_tmp5D9,sizeof(char),3));}case '\b': _LL1: {const char*_tmp5DA;
return(_tmp5DA="\\b",_tag_dyneither(_tmp5DA,sizeof(char),3));}case '\f': _LL2: {
const char*_tmp5DB;return(_tmp5DB="\\f",_tag_dyneither(_tmp5DB,sizeof(char),3));}
case '\n': _LL3: {const char*_tmp5DC;return(_tmp5DC="\\n",_tag_dyneither(_tmp5DC,
sizeof(char),3));}case '\r': _LL4: {const char*_tmp5DD;return(_tmp5DD="\\r",
_tag_dyneither(_tmp5DD,sizeof(char),3));}case '\t': _LL5: {const char*_tmp5DE;
return(_tmp5DE="\\t",_tag_dyneither(_tmp5DE,sizeof(char),3));}case '\v': _LL6: {
const char*_tmp5DF;return(_tmp5DF="\\v",_tag_dyneither(_tmp5DF,sizeof(char),3));}
case '\\': _LL7: {const char*_tmp5E0;return(_tmp5E0="\\\\",_tag_dyneither(_tmp5E0,
sizeof(char),3));}case '"': _LL8: {const char*_tmp5E1;return(_tmp5E1="\"",
_tag_dyneither(_tmp5E1,sizeof(char),2));}case '\'': _LL9: {const char*_tmp5E2;
return(_tmp5E2="\\'",_tag_dyneither(_tmp5E2,sizeof(char),3));}default: _LLA: if(c
>= ' '  && c <= '~'){struct _dyneither_ptr _tmpC=Cyc_Core_new_string(2);{char _tmp5E5;
char _tmp5E4;struct _dyneither_ptr _tmp5E3;(_tmp5E3=_dyneither_ptr_plus(_tmpC,
sizeof(char),0),((_tmp5E4=*((char*)_check_dyneither_subscript(_tmp5E3,sizeof(
char),0)),((_tmp5E5=c,((_get_dyneither_size(_tmp5E3,sizeof(char))== 1  && (
_tmp5E4 == '\000'  && _tmp5E5 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5E3.curr)=
_tmp5E5)))))));}return(struct _dyneither_ptr)_tmpC;}else{struct _dyneither_ptr
_tmp10=Cyc_Core_new_string(5);int j=0;{char _tmp5E8;char _tmp5E7;struct
_dyneither_ptr _tmp5E6;(_tmp5E6=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((
_tmp5E7=*((char*)_check_dyneither_subscript(_tmp5E6,sizeof(char),0)),((_tmp5E8='\\',((
_get_dyneither_size(_tmp5E6,sizeof(char))== 1  && (_tmp5E7 == '\000'  && _tmp5E8 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp5E6.curr)=_tmp5E8)))))));}{char _tmp5EB;char
_tmp5EA;struct _dyneither_ptr _tmp5E9;(_tmp5E9=_dyneither_ptr_plus(_tmp10,sizeof(
char),j ++),((_tmp5EA=*((char*)_check_dyneither_subscript(_tmp5E9,sizeof(char),0)),((
_tmp5EB=(char)('0' + ((unsigned char)c >> 6 & 3)),((_get_dyneither_size(_tmp5E9,
sizeof(char))== 1  && (_tmp5EA == '\000'  && _tmp5EB != '\000')?_throw_arraybounds():
1,*((char*)_tmp5E9.curr)=_tmp5EB)))))));}{char _tmp5EE;char _tmp5ED;struct
_dyneither_ptr _tmp5EC;(_tmp5EC=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((
_tmp5ED=*((char*)_check_dyneither_subscript(_tmp5EC,sizeof(char),0)),((_tmp5EE=(
char)('0' + (c >> 3 & 7)),((_get_dyneither_size(_tmp5EC,sizeof(char))== 1  && (
_tmp5ED == '\000'  && _tmp5EE != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5EC.curr)=
_tmp5EE)))))));}{char _tmp5F1;char _tmp5F0;struct _dyneither_ptr _tmp5EF;(_tmp5EF=
_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp5F0=*((char*)
_check_dyneither_subscript(_tmp5EF,sizeof(char),0)),((_tmp5F1=(char)('0' + (c & 7)),((
_get_dyneither_size(_tmp5EF,sizeof(char))== 1  && (_tmp5F0 == '\000'  && _tmp5F1 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp5EF.curr)=_tmp5F1)))))));}return(struct
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
_LL22: if(_tmp1E != '\a')goto _LL24;_LL23:{char _tmp5F4;char _tmp5F3;struct
_dyneither_ptr _tmp5F2;(_tmp5F2=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5F3=*((
char*)_check_dyneither_subscript(_tmp5F2,sizeof(char),0)),((_tmp5F4='\\',((
_get_dyneither_size(_tmp5F2,sizeof(char))== 1  && (_tmp5F3 == '\000'  && _tmp5F4 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp5F2.curr)=_tmp5F4)))))));}{char _tmp5F7;char
_tmp5F6;struct _dyneither_ptr _tmp5F5;(_tmp5F5=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp5F6=*((char*)_check_dyneither_subscript(_tmp5F5,sizeof(char),0)),((
_tmp5F7='a',((_get_dyneither_size(_tmp5F5,sizeof(char))== 1  && (_tmp5F6 == '\000'
 && _tmp5F7 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5F5.curr)=_tmp5F7)))))));}
goto _LL21;_LL24: if(_tmp1E != '\b')goto _LL26;_LL25:{char _tmp5FA;char _tmp5F9;struct
_dyneither_ptr _tmp5F8;(_tmp5F8=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5F9=*((
char*)_check_dyneither_subscript(_tmp5F8,sizeof(char),0)),((_tmp5FA='\\',((
_get_dyneither_size(_tmp5F8,sizeof(char))== 1  && (_tmp5F9 == '\000'  && _tmp5FA != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp5F8.curr)=_tmp5FA)))))));}{char _tmp5FD;char
_tmp5FC;struct _dyneither_ptr _tmp5FB;(_tmp5FB=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp5FC=*((char*)_check_dyneither_subscript(_tmp5FB,sizeof(char),0)),((
_tmp5FD='b',((_get_dyneither_size(_tmp5FB,sizeof(char))== 1  && (_tmp5FC == '\000'
 && _tmp5FD != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5FB.curr)=_tmp5FD)))))));}
goto _LL21;_LL26: if(_tmp1E != '\f')goto _LL28;_LL27:{char _tmp600;char _tmp5FF;struct
_dyneither_ptr _tmp5FE;(_tmp5FE=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5FF=*((
char*)_check_dyneither_subscript(_tmp5FE,sizeof(char),0)),((_tmp600='\\',((
_get_dyneither_size(_tmp5FE,sizeof(char))== 1  && (_tmp5FF == '\000'  && _tmp600 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp5FE.curr)=_tmp600)))))));}{char _tmp603;char
_tmp602;struct _dyneither_ptr _tmp601;(_tmp601=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp602=*((char*)_check_dyneither_subscript(_tmp601,sizeof(char),0)),((
_tmp603='f',((_get_dyneither_size(_tmp601,sizeof(char))== 1  && (_tmp602 == '\000'
 && _tmp603 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp601.curr)=_tmp603)))))));}
goto _LL21;_LL28: if(_tmp1E != '\n')goto _LL2A;_LL29:{char _tmp606;char _tmp605;struct
_dyneither_ptr _tmp604;(_tmp604=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp605=*((
char*)_check_dyneither_subscript(_tmp604,sizeof(char),0)),((_tmp606='\\',((
_get_dyneither_size(_tmp604,sizeof(char))== 1  && (_tmp605 == '\000'  && _tmp606 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp604.curr)=_tmp606)))))));}{char _tmp609;char
_tmp608;struct _dyneither_ptr _tmp607;(_tmp607=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp608=*((char*)_check_dyneither_subscript(_tmp607,sizeof(char),0)),((
_tmp609='n',((_get_dyneither_size(_tmp607,sizeof(char))== 1  && (_tmp608 == '\000'
 && _tmp609 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp607.curr)=_tmp609)))))));}
goto _LL21;_LL2A: if(_tmp1E != '\r')goto _LL2C;_LL2B:{char _tmp60C;char _tmp60B;struct
_dyneither_ptr _tmp60A;(_tmp60A=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp60B=*((
char*)_check_dyneither_subscript(_tmp60A,sizeof(char),0)),((_tmp60C='\\',((
_get_dyneither_size(_tmp60A,sizeof(char))== 1  && (_tmp60B == '\000'  && _tmp60C != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp60A.curr)=_tmp60C)))))));}{char _tmp60F;char
_tmp60E;struct _dyneither_ptr _tmp60D;(_tmp60D=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp60E=*((char*)_check_dyneither_subscript(_tmp60D,sizeof(char),0)),((
_tmp60F='r',((_get_dyneither_size(_tmp60D,sizeof(char))== 1  && (_tmp60E == '\000'
 && _tmp60F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp60D.curr)=_tmp60F)))))));}
goto _LL21;_LL2C: if(_tmp1E != '\t')goto _LL2E;_LL2D:{char _tmp612;char _tmp611;struct
_dyneither_ptr _tmp610;(_tmp610=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp611=*((
char*)_check_dyneither_subscript(_tmp610,sizeof(char),0)),((_tmp612='\\',((
_get_dyneither_size(_tmp610,sizeof(char))== 1  && (_tmp611 == '\000'  && _tmp612 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp610.curr)=_tmp612)))))));}{char _tmp615;char
_tmp614;struct _dyneither_ptr _tmp613;(_tmp613=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp614=*((char*)_check_dyneither_subscript(_tmp613,sizeof(char),0)),((
_tmp615='t',((_get_dyneither_size(_tmp613,sizeof(char))== 1  && (_tmp614 == '\000'
 && _tmp615 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp613.curr)=_tmp615)))))));}
goto _LL21;_LL2E: if(_tmp1E != '\v')goto _LL30;_LL2F:{char _tmp618;char _tmp617;struct
_dyneither_ptr _tmp616;(_tmp616=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp617=*((
char*)_check_dyneither_subscript(_tmp616,sizeof(char),0)),((_tmp618='\\',((
_get_dyneither_size(_tmp616,sizeof(char))== 1  && (_tmp617 == '\000'  && _tmp618 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp616.curr)=_tmp618)))))));}{char _tmp61B;char
_tmp61A;struct _dyneither_ptr _tmp619;(_tmp619=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp61A=*((char*)_check_dyneither_subscript(_tmp619,sizeof(char),0)),((
_tmp61B='v',((_get_dyneither_size(_tmp619,sizeof(char))== 1  && (_tmp61A == '\000'
 && _tmp61B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp619.curr)=_tmp61B)))))));}
goto _LL21;_LL30: if(_tmp1E != '\\')goto _LL32;_LL31:{char _tmp61E;char _tmp61D;struct
_dyneither_ptr _tmp61C;(_tmp61C=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp61D=*((
char*)_check_dyneither_subscript(_tmp61C,sizeof(char),0)),((_tmp61E='\\',((
_get_dyneither_size(_tmp61C,sizeof(char))== 1  && (_tmp61D == '\000'  && _tmp61E != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp61C.curr)=_tmp61E)))))));}{char _tmp621;char
_tmp620;struct _dyneither_ptr _tmp61F;(_tmp61F=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp620=*((char*)_check_dyneither_subscript(_tmp61F,sizeof(char),0)),((
_tmp621='\\',((_get_dyneither_size(_tmp61F,sizeof(char))== 1  && (_tmp620 == '\000'
 && _tmp621 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp61F.curr)=_tmp621)))))));}
goto _LL21;_LL32: if(_tmp1E != '"')goto _LL34;_LL33:{char _tmp624;char _tmp623;struct
_dyneither_ptr _tmp622;(_tmp622=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp623=*((
char*)_check_dyneither_subscript(_tmp622,sizeof(char),0)),((_tmp624='\\',((
_get_dyneither_size(_tmp622,sizeof(char))== 1  && (_tmp623 == '\000'  && _tmp624 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp622.curr)=_tmp624)))))));}{char _tmp627;char
_tmp626;struct _dyneither_ptr _tmp625;(_tmp625=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp626=*((char*)_check_dyneither_subscript(_tmp625,sizeof(char),0)),((
_tmp627='"',((_get_dyneither_size(_tmp625,sizeof(char))== 1  && (_tmp626 == '\000'
 && _tmp627 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp625.curr)=_tmp627)))))));}
goto _LL21;_LL34:;_LL35: if(_tmp1E >= ' '  && _tmp1E <= '~'){char _tmp62A;char _tmp629;
struct _dyneither_ptr _tmp628;(_tmp628=_dyneither_ptr_plus(t,sizeof(char),j ++),((
_tmp629=*((char*)_check_dyneither_subscript(_tmp628,sizeof(char),0)),((_tmp62A=
_tmp1E,((_get_dyneither_size(_tmp628,sizeof(char))== 1  && (_tmp629 == '\000'  && 
_tmp62A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp628.curr)=_tmp62A)))))));}
else{{char _tmp62D;char _tmp62C;struct _dyneither_ptr _tmp62B;(_tmp62B=
_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp62C=*((char*)
_check_dyneither_subscript(_tmp62B,sizeof(char),0)),((_tmp62D='\\',((
_get_dyneither_size(_tmp62B,sizeof(char))== 1  && (_tmp62C == '\000'  && _tmp62D != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp62B.curr)=_tmp62D)))))));}{char _tmp630;char
_tmp62F;struct _dyneither_ptr _tmp62E;(_tmp62E=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp62F=*((char*)_check_dyneither_subscript(_tmp62E,sizeof(char),0)),((
_tmp630=(char)('0' + (_tmp1E >> 6 & 7)),((_get_dyneither_size(_tmp62E,sizeof(char))
== 1  && (_tmp62F == '\000'  && _tmp630 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp62E.curr)=_tmp630)))))));}{char _tmp633;char _tmp632;struct _dyneither_ptr
_tmp631;(_tmp631=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp632=*((char*)
_check_dyneither_subscript(_tmp631,sizeof(char),0)),((_tmp633=(char)('0' + (
_tmp1E >> 3 & 7)),((_get_dyneither_size(_tmp631,sizeof(char))== 1  && (_tmp632 == '\000'
 && _tmp633 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp631.curr)=_tmp633)))))));}{
char _tmp636;char _tmp635;struct _dyneither_ptr _tmp634;(_tmp634=_dyneither_ptr_plus(
t,sizeof(char),j ++),((_tmp635=*((char*)_check_dyneither_subscript(_tmp634,
sizeof(char),0)),((_tmp636=(char)('0' + (_tmp1E & 7)),((_get_dyneither_size(
_tmp634,sizeof(char))== 1  && (_tmp635 == '\000'  && _tmp636 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp634.curr)=_tmp636)))))));}}goto _LL21;_LL21:;}}
return(struct _dyneither_ptr)t;}}}}static char _tmp64[9]="restrict";static struct
_dyneither_ptr Cyc_Absynpp_restrict_string={_tmp64,_tmp64,_tmp64 + 9};static char
_tmp65[9]="volatile";static struct _dyneither_ptr Cyc_Absynpp_volatile_string={
_tmp65,_tmp65,_tmp65 + 9};static char _tmp66[6]="const";static struct _dyneither_ptr
Cyc_Absynpp_const_str={_tmp66,_tmp66,_tmp66 + 6};static struct _dyneither_ptr*Cyc_Absynpp_restrict_sp=&
Cyc_Absynpp_restrict_string;static struct _dyneither_ptr*Cyc_Absynpp_volatile_sp=&
Cyc_Absynpp_volatile_string;static struct _dyneither_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq);struct Cyc_PP_Doc*
Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){struct Cyc_List_List*l=0;if(tq.q_restrict){
struct Cyc_List_List*_tmp637;l=((_tmp637=_cycalloc(sizeof(*_tmp637)),((_tmp637->hd=
Cyc_Absynpp_restrict_sp,((_tmp637->tl=l,_tmp637))))));}if(tq.q_volatile){struct
Cyc_List_List*_tmp638;l=((_tmp638=_cycalloc(sizeof(*_tmp638)),((_tmp638->hd=Cyc_Absynpp_volatile_sp,((
_tmp638->tl=l,_tmp638))))));}if(tq.print_const){struct Cyc_List_List*_tmp639;l=((
_tmp639=_cycalloc(sizeof(*_tmp639)),((_tmp639->hd=Cyc_Absynpp_const_sp,((_tmp639->tl=
l,_tmp639))))));}{const char*_tmp63C;const char*_tmp63B;const char*_tmp63A;return
Cyc_PP_egroup(((_tmp63A="",_tag_dyneither(_tmp63A,sizeof(char),1))),((_tmp63B=" ",
_tag_dyneither(_tmp63B,sizeof(char),2))),((_tmp63C=" ",_tag_dyneither(_tmp63C,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));}}
struct _dyneither_ptr Cyc_Absynpp_kind2string(enum Cyc_Absyn_Kind k);struct
_dyneither_ptr Cyc_Absynpp_kind2string(enum Cyc_Absyn_Kind k){switch(k){case Cyc_Absyn_AnyKind:
_LL36: {const char*_tmp63D;return(_tmp63D="A",_tag_dyneither(_tmp63D,sizeof(char),
2));}case Cyc_Absyn_MemKind: _LL37: {const char*_tmp63E;return(_tmp63E="M",
_tag_dyneither(_tmp63E,sizeof(char),2));}case Cyc_Absyn_BoxKind: _LL38: {const char*
_tmp63F;return(_tmp63F="B",_tag_dyneither(_tmp63F,sizeof(char),2));}case Cyc_Absyn_RgnKind:
_LL39: {const char*_tmp640;return(_tmp640="R",_tag_dyneither(_tmp640,sizeof(char),
2));}case Cyc_Absyn_UniqueRgnKind: _LL3A: {const char*_tmp641;return(_tmp641="UR",
_tag_dyneither(_tmp641,sizeof(char),3));}case Cyc_Absyn_TopRgnKind: _LL3B: {const
char*_tmp642;return(_tmp642="TR",_tag_dyneither(_tmp642,sizeof(char),3));}case
Cyc_Absyn_EffKind: _LL3C: {const char*_tmp643;return(_tmp643="E",_tag_dyneither(
_tmp643,sizeof(char),2));}case Cyc_Absyn_IntKind: _LL3D: {const char*_tmp644;return(
_tmp644="I",_tag_dyneither(_tmp644,sizeof(char),2));}}}struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(
enum Cyc_Absyn_Kind k);struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(enum Cyc_Absyn_Kind k){
return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}struct _dyneither_ptr Cyc_Absynpp_kindbound2string(
void*c);struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){void*_tmp75=Cyc_Absyn_compress_kb(
c);enum Cyc_Absyn_Kind _tmp77;enum Cyc_Absyn_Kind _tmp7A;_LL40: {struct Cyc_Absyn_Eq_kb_struct*
_tmp76=(struct Cyc_Absyn_Eq_kb_struct*)_tmp75;if(_tmp76->tag != 0)goto _LL42;else{
_tmp77=_tmp76->f1;}}_LL41: return Cyc_Absynpp_kind2string(_tmp77);_LL42: {struct
Cyc_Absyn_Unknown_kb_struct*_tmp78=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp75;
if(_tmp78->tag != 1)goto _LL44;}_LL43: if(Cyc_PP_tex_output){const char*_tmp645;
return(_tmp645="{?}",_tag_dyneither(_tmp645,sizeof(char),4));}else{const char*
_tmp646;return(_tmp646="?",_tag_dyneither(_tmp646,sizeof(char),2));}_LL44: {
struct Cyc_Absyn_Less_kb_struct*_tmp79=(struct Cyc_Absyn_Less_kb_struct*)_tmp75;
if(_tmp79->tag != 2)goto _LL3F;else{_tmp7A=_tmp79->f2;}}_LL45: return Cyc_Absynpp_kind2string(
_tmp7A);_LL3F:;}struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c);struct Cyc_PP_Doc*
Cyc_Absynpp_kindbound2doc(void*c){void*_tmp7D=Cyc_Absyn_compress_kb(c);enum Cyc_Absyn_Kind
_tmp7F;enum Cyc_Absyn_Kind _tmp82;_LL47: {struct Cyc_Absyn_Eq_kb_struct*_tmp7E=(
struct Cyc_Absyn_Eq_kb_struct*)_tmp7D;if(_tmp7E->tag != 0)goto _LL49;else{_tmp7F=
_tmp7E->f1;}}_LL48: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp7F));_LL49: {
struct Cyc_Absyn_Unknown_kb_struct*_tmp80=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp7D;if(_tmp80->tag != 1)goto _LL4B;}_LL4A: if(Cyc_PP_tex_output){const char*
_tmp647;return Cyc_PP_text_width(((_tmp647="{?}",_tag_dyneither(_tmp647,sizeof(
char),4))),1);}else{const char*_tmp648;return Cyc_PP_text(((_tmp648="?",
_tag_dyneither(_tmp648,sizeof(char),2))));}_LL4B: {struct Cyc_Absyn_Less_kb_struct*
_tmp81=(struct Cyc_Absyn_Less_kb_struct*)_tmp7D;if(_tmp81->tag != 2)goto _LL46;
else{_tmp82=_tmp81->f2;}}_LL4C: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp82));
_LL46:;}struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts);struct Cyc_PP_Doc*
Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){const char*_tmp64B;const char*_tmp64A;
const char*_tmp649;return Cyc_PP_egroup(((_tmp649="<",_tag_dyneither(_tmp649,
sizeof(char),2))),((_tmp64A=">",_tag_dyneither(_tmp64A,sizeof(char),2))),((
_tmp64B=",",_tag_dyneither(_tmp64B,sizeof(char),2))),((struct Cyc_List_List*(*)(
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
_tmp64E;struct Cyc_PP_Doc*_tmp64D[3];return(_tmp64D[2]=Cyc_Absynpp_kind2doc(
_tmp8F),((_tmp64D[1]=Cyc_PP_text(((_tmp64E="::",_tag_dyneither(_tmp64E,sizeof(
char),3)))),((_tmp64D[0]=Cyc_PP_textptr(tv->name),Cyc_PP_cat(_tag_dyneither(
_tmp64D,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL4D:;}struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(
struct Cyc_List_List*tvs);struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*
tvs){const char*_tmp651;const char*_tmp650;const char*_tmp64F;return Cyc_PP_egroup(((
_tmp64F="<",_tag_dyneither(_tmp64F,sizeof(char),2))),((_tmp650=">",
_tag_dyneither(_tmp650,sizeof(char),2))),((_tmp651=",",_tag_dyneither(_tmp651,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));}static struct
_dyneither_ptr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv);static struct
_dyneither_ptr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv){return tv->name;}
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs);struct Cyc_PP_Doc*
Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);{const char*_tmp654;const char*_tmp653;const char*
_tmp652;return Cyc_PP_egroup(((_tmp652="<",_tag_dyneither(_tmp652,sizeof(char),2))),((
_tmp653=">",_tag_dyneither(_tmp653,sizeof(char),2))),((_tmp654=",",
_tag_dyneither(_tmp654,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((
struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,tvs)));}}struct _tuple16{
struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct
_tuple16*t);struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple16*t){return Cyc_Absynpp_tqtd2doc((*
t).f1,(*t).f2,0);}struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts);
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){const char*_tmp657;
const char*_tmp656;const char*_tmp655;return Cyc_PP_group(((_tmp655="(",
_tag_dyneither(_tmp655,sizeof(char),2))),((_tmp656=")",_tag_dyneither(_tmp656,
sizeof(char),2))),((_tmp657=",",_tag_dyneither(_tmp657,sizeof(char),2))),((
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
if(_tmpA0->tag != 1)goto _LL62;}_LL61: {const char*_tmp658;return Cyc_PP_text(((
_tmp658=" _stdcall ",_tag_dyneither(_tmp658,sizeof(char),11))));}_LL62: {struct
Cyc_Absyn_Cdecl_att_struct*_tmpA1=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp9F;if(
_tmpA1->tag != 2)goto _LL64;}_LL63: {const char*_tmp659;return Cyc_PP_text(((_tmp659=" _cdecl ",
_tag_dyneither(_tmp659,sizeof(char),9))));}_LL64: {struct Cyc_Absyn_Fastcall_att_struct*
_tmpA2=(struct Cyc_Absyn_Fastcall_att_struct*)_tmp9F;if(_tmpA2->tag != 3)goto _LL66;}
_LL65: {const char*_tmp65A;return Cyc_PP_text(((_tmp65A=" _fastcall ",
_tag_dyneither(_tmp65A,sizeof(char),12))));}_LL66:;_LL67: goto _LL5F;_LL5F:;}
return Cyc_PP_nil_doc();}struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*
atts);struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){int
hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){void*
_tmpA6=(void*)atts2->hd;_LL69: {struct Cyc_Absyn_Stdcall_att_struct*_tmpA7=(
struct Cyc_Absyn_Stdcall_att_struct*)_tmpA6;if(_tmpA7->tag != 1)goto _LL6B;}_LL6A:
goto _LL6C;_LL6B: {struct Cyc_Absyn_Cdecl_att_struct*_tmpA8=(struct Cyc_Absyn_Cdecl_att_struct*)
_tmpA6;if(_tmpA8->tag != 2)goto _LL6D;}_LL6C: goto _LL6E;_LL6D: {struct Cyc_Absyn_Fastcall_att_struct*
_tmpA9=(struct Cyc_Absyn_Fastcall_att_struct*)_tmpA6;if(_tmpA9->tag != 3)goto _LL6F;}
_LL6E: goto _LL68;_LL6F:;_LL70: hasatt=1;goto _LL68;_LL68:;}}if(!hasatt)return Cyc_PP_nil_doc();{
const char*_tmp665;const char*_tmp664;const char*_tmp663;const char*_tmp662;const
char*_tmp661;struct Cyc_PP_Doc*_tmp660[3];return(_tmp660[2]=Cyc_PP_text(((_tmp665=")",
_tag_dyneither(_tmp665,sizeof(char),2)))),((_tmp660[1]=Cyc_PP_group(((_tmp664="",
_tag_dyneither(_tmp664,sizeof(char),1))),((_tmp663="",_tag_dyneither(_tmp663,
sizeof(char),1))),((_tmp662=" ",_tag_dyneither(_tmp662,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_noncallatt2doc,atts)),((_tmp660[0]=Cyc_PP_text(((_tmp661=" __declspec(",
_tag_dyneither(_tmp661,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp660,
sizeof(struct Cyc_PP_Doc*),3)))))));}}struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a);
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){return Cyc_PP_text(Cyc_Absyn_attribute2string(
a));}struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts);struct Cyc_PP_Doc*
Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){if(atts == 0)return Cyc_PP_nil_doc();
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL71: return Cyc_Absynpp_noncallconv2doc(
atts);case Cyc_Cyclone_Gcc_c: _LL72: {const char*_tmp66E;const char*_tmp66D;const
char*_tmp66C;const char*_tmp66B;struct Cyc_PP_Doc*_tmp66A[2];return(_tmp66A[1]=Cyc_PP_group(((
_tmp66E="((",_tag_dyneither(_tmp66E,sizeof(char),3))),((_tmp66D="))",
_tag_dyneither(_tmp66D,sizeof(char),3))),((_tmp66C=",",_tag_dyneither(_tmp66C,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts)),((_tmp66A[0]=Cyc_PP_text(((
_tmp66B=" __attribute__",_tag_dyneither(_tmp66B,sizeof(char),15)))),Cyc_PP_cat(
_tag_dyneither(_tmp66A,sizeof(struct Cyc_PP_Doc*),2)))));}}}int Cyc_Absynpp_next_is_pointer(
struct Cyc_List_List*tms);int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{void*_tmpB5=(void*)tms->hd;_LL75: {struct Cyc_Absyn_Pointer_mod_struct*
_tmpB6=(struct Cyc_Absyn_Pointer_mod_struct*)_tmpB5;if(_tmpB6->tag != 2)goto _LL77;}
_LL76: return 1;_LL77: {struct Cyc_Absyn_Attributes_mod_struct*_tmpB7=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpB5;if(_tmpB7->tag != 5)goto _LL79;}_LL78: switch(Cyc_Cyclone_c_compiler){case
Cyc_Cyclone_Gcc_c: _LL7B: return 0;default: _LL7C: return Cyc_Absynpp_next_is_pointer(
tms->tl);}_LL79:;_LL7A: return 0;_LL74:;}}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(
void*t);static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;static struct Cyc_PP_Doc*
Cyc_Absynpp_question();static struct Cyc_PP_Doc*Cyc_Absynpp_question(){if(!((
unsigned int)Cyc_Absynpp_cache_question)){if(Cyc_PP_tex_output){const char*
_tmp66F;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((
_tmp66F="{?}",_tag_dyneither(_tmp66F,sizeof(char),4))),1);}else{const char*
_tmp670;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp670="?",
_tag_dyneither(_tmp670,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)_check_null(
Cyc_Absynpp_cache_question);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb();static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned int)Cyc_Absynpp_cache_lb)){if(Cyc_PP_tex_output){const char*
_tmp671;Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp671="{\\lb}",
_tag_dyneither(_tmp671,sizeof(char),6))),1);}else{const char*_tmp672;Cyc_Absynpp_cache_lb=(
struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp672="{",_tag_dyneither(_tmp672,sizeof(char),
2))));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}static struct
Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;static struct Cyc_PP_Doc*Cyc_Absynpp_rb();static
struct Cyc_PP_Doc*Cyc_Absynpp_rb(){if(!((unsigned int)Cyc_Absynpp_cache_rb)){if(
Cyc_PP_tex_output){const char*_tmp673;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((
_tmp673="{\\rb}",_tag_dyneither(_tmp673,sizeof(char),6))),1);}else{const char*
_tmp674;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp674="}",
_tag_dyneither(_tmp674,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)_check_null(
Cyc_Absynpp_cache_rb);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;static
struct Cyc_PP_Doc*Cyc_Absynpp_dollar();static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){if(Cyc_PP_tex_output){const char*
_tmp675;Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp675="\\$",
_tag_dyneither(_tmp675,sizeof(char),3))),1);}else{const char*_tmp676;Cyc_Absynpp_cache_dollar=(
struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp676="$",_tag_dyneither(_tmp676,sizeof(char),
2))));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}struct
Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*
ss);struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*
ss){struct Cyc_PP_Doc*_tmp677[3];return(_tmp677[2]=Cyc_Absynpp_rb(),((_tmp677[1]=
Cyc_PP_seq(sep,ss),((_tmp677[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(
_tmp677,sizeof(struct Cyc_PP_Doc*),3)))))));}static void Cyc_Absynpp_print_tms(
struct Cyc_List_List*tms);static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){void*_tmpC1=(void*)tms->hd;void*_tmpC5;struct Cyc_List_List*
_tmpC7;_LL7F: {struct Cyc_Absyn_Carray_mod_struct*_tmpC2=(struct Cyc_Absyn_Carray_mod_struct*)
_tmpC1;if(_tmpC2->tag != 0)goto _LL81;}_LL80:{const char*_tmp67A;void*_tmp679;(
_tmp679=0,Cyc_fprintf(Cyc_stderr,((_tmp67A="Carray_mod ",_tag_dyneither(_tmp67A,
sizeof(char),12))),_tag_dyneither(_tmp679,sizeof(void*),0)));}goto _LL7E;_LL81: {
struct Cyc_Absyn_ConstArray_mod_struct*_tmpC3=(struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpC1;if(_tmpC3->tag != 1)goto _LL83;}_LL82:{const char*_tmp67D;void*_tmp67C;(
_tmp67C=0,Cyc_fprintf(Cyc_stderr,((_tmp67D="ConstArray_mod ",_tag_dyneither(
_tmp67D,sizeof(char),16))),_tag_dyneither(_tmp67C,sizeof(void*),0)));}goto _LL7E;
_LL83: {struct Cyc_Absyn_Function_mod_struct*_tmpC4=(struct Cyc_Absyn_Function_mod_struct*)
_tmpC1;if(_tmpC4->tag != 3)goto _LL85;else{_tmpC5=(void*)_tmpC4->f1;{struct Cyc_Absyn_WithTypes_struct*
_tmpC6=(struct Cyc_Absyn_WithTypes_struct*)_tmpC5;if(_tmpC6->tag != 1)goto _LL85;
else{_tmpC7=_tmpC6->f1;}}}}_LL84:{const char*_tmp680;void*_tmp67F;(_tmp67F=0,Cyc_fprintf(
Cyc_stderr,((_tmp680="Function_mod(",_tag_dyneither(_tmp680,sizeof(char),14))),
_tag_dyneither(_tmp67F,sizeof(void*),0)));}for(0;_tmpC7 != 0;_tmpC7=_tmpC7->tl){
struct Cyc_Core_Opt*_tmpD2=(*((struct _tuple7*)_tmpC7->hd)).f1;if(_tmpD2 == 0){
const char*_tmp683;void*_tmp682;(_tmp682=0,Cyc_fprintf(Cyc_stderr,((_tmp683="?",
_tag_dyneither(_tmp683,sizeof(char),2))),_tag_dyneither(_tmp682,sizeof(void*),0)));}
else{void*_tmp684;(_tmp684=0,Cyc_fprintf(Cyc_stderr,*((struct _dyneither_ptr*)
_tmpD2->v),_tag_dyneither(_tmp684,sizeof(void*),0)));}if(_tmpC7->tl != 0){const
char*_tmp687;void*_tmp686;(_tmp686=0,Cyc_fprintf(Cyc_stderr,((_tmp687=",",
_tag_dyneither(_tmp687,sizeof(char),2))),_tag_dyneither(_tmp686,sizeof(void*),0)));}}{
const char*_tmp68A;void*_tmp689;(_tmp689=0,Cyc_fprintf(Cyc_stderr,((_tmp68A=") ",
_tag_dyneither(_tmp68A,sizeof(char),3))),_tag_dyneither(_tmp689,sizeof(void*),0)));}
goto _LL7E;_LL85: {struct Cyc_Absyn_Function_mod_struct*_tmpC8=(struct Cyc_Absyn_Function_mod_struct*)
_tmpC1;if(_tmpC8->tag != 3)goto _LL87;}_LL86:{const char*_tmp68D;void*_tmp68C;(
_tmp68C=0,Cyc_fprintf(Cyc_stderr,((_tmp68D="Function_mod()",_tag_dyneither(
_tmp68D,sizeof(char),15))),_tag_dyneither(_tmp68C,sizeof(void*),0)));}goto _LL7E;
_LL87: {struct Cyc_Absyn_Attributes_mod_struct*_tmpC9=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpC1;if(_tmpC9->tag != 5)goto _LL89;}_LL88:{const char*_tmp690;void*_tmp68F;(
_tmp68F=0,Cyc_fprintf(Cyc_stderr,((_tmp690="Attributes_mod ",_tag_dyneither(
_tmp690,sizeof(char),16))),_tag_dyneither(_tmp68F,sizeof(void*),0)));}goto _LL7E;
_LL89: {struct Cyc_Absyn_TypeParams_mod_struct*_tmpCA=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpC1;if(_tmpCA->tag != 4)goto _LL8B;}_LL8A:{const char*_tmp693;void*_tmp692;(
_tmp692=0,Cyc_fprintf(Cyc_stderr,((_tmp693="TypeParams_mod ",_tag_dyneither(
_tmp693,sizeof(char),16))),_tag_dyneither(_tmp692,sizeof(void*),0)));}goto _LL7E;
_LL8B: {struct Cyc_Absyn_Pointer_mod_struct*_tmpCB=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmpC1;if(_tmpCB->tag != 2)goto _LL7E;}_LL8C:{const char*_tmp696;void*_tmp695;(
_tmp695=0,Cyc_fprintf(Cyc_stderr,((_tmp696="Pointer_mod ",_tag_dyneither(_tmp696,
sizeof(char),13))),_tag_dyneither(_tmp695,sizeof(void*),0)));}goto _LL7E;_LL7E:;}{
const char*_tmp699;void*_tmp698;(_tmp698=0,Cyc_fprintf(Cyc_stderr,((_tmp699="\n",
_tag_dyneither(_tmp699,sizeof(char),2))),_tag_dyneither(_tmp698,sizeof(void*),0)));}}
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct
Cyc_List_List*tms);struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct
Cyc_PP_Doc*d,struct Cyc_List_List*tms){if(tms == 0)return d;{struct Cyc_PP_Doc*rest=
Cyc_Absynpp_dtms2doc(0,d,tms->tl);const char*_tmp69E;const char*_tmp69D;struct Cyc_PP_Doc*
_tmp69C[3];struct Cyc_PP_Doc*p_rest=(_tmp69C[2]=Cyc_PP_text(((_tmp69E=")",
_tag_dyneither(_tmp69E,sizeof(char),2)))),((_tmp69C[1]=rest,((_tmp69C[0]=Cyc_PP_text(((
_tmp69D="(",_tag_dyneither(_tmp69D,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp69C,sizeof(struct Cyc_PP_Doc*),3)))))));void*_tmpE4=(void*)tms->hd;union Cyc_Absyn_Constraint*
_tmpE6;struct Cyc_Absyn_Exp*_tmpE8;union Cyc_Absyn_Constraint*_tmpE9;void*_tmpEB;
struct Cyc_List_List*_tmpED;struct Cyc_List_List*_tmpEF;struct Cyc_Position_Segment*
_tmpF0;int _tmpF1;struct Cyc_Absyn_PtrAtts _tmpF3;void*_tmpF4;union Cyc_Absyn_Constraint*
_tmpF5;union Cyc_Absyn_Constraint*_tmpF6;union Cyc_Absyn_Constraint*_tmpF7;struct
Cyc_Absyn_Tqual _tmpF8;_LL8E: {struct Cyc_Absyn_Carray_mod_struct*_tmpE5=(struct
Cyc_Absyn_Carray_mod_struct*)_tmpE4;if(_tmpE5->tag != 0)goto _LL90;else{_tmpE6=
_tmpE5->f1;}}_LL8F: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{const
char*_tmp6A3;const char*_tmp6A2;struct Cyc_PP_Doc*_tmp6A1[2];return(_tmp6A1[1]=((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpE6)?Cyc_PP_text(((
_tmp6A2="[]@zeroterm",_tag_dyneither(_tmp6A2,sizeof(char),12)))): Cyc_PP_text(((
_tmp6A3="[]",_tag_dyneither(_tmp6A3,sizeof(char),3)))),((_tmp6A1[0]=rest,Cyc_PP_cat(
_tag_dyneither(_tmp6A1,sizeof(struct Cyc_PP_Doc*),2)))));}_LL90: {struct Cyc_Absyn_ConstArray_mod_struct*
_tmpE7=(struct Cyc_Absyn_ConstArray_mod_struct*)_tmpE4;if(_tmpE7->tag != 1)goto
_LL92;else{_tmpE8=_tmpE7->f1;_tmpE9=_tmpE7->f2;}}_LL91: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{const char*_tmp6AA;const char*_tmp6A9;const char*_tmp6A8;
struct Cyc_PP_Doc*_tmp6A7[4];return(_tmp6A7[3]=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmpE9)?Cyc_PP_text(((_tmp6A9="]@zeroterm",
_tag_dyneither(_tmp6A9,sizeof(char),11)))): Cyc_PP_text(((_tmp6AA="]",
_tag_dyneither(_tmp6AA,sizeof(char),2)))),((_tmp6A7[2]=Cyc_Absynpp_exp2doc(
_tmpE8),((_tmp6A7[1]=Cyc_PP_text(((_tmp6A8="[",_tag_dyneither(_tmp6A8,sizeof(
char),2)))),((_tmp6A7[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6A7,sizeof(struct Cyc_PP_Doc*),
4)))))))));}_LL92: {struct Cyc_Absyn_Function_mod_struct*_tmpEA=(struct Cyc_Absyn_Function_mod_struct*)
_tmpE4;if(_tmpEA->tag != 3)goto _LL94;else{_tmpEB=(void*)_tmpEA->f1;}}_LL93: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{void*_tmp100=_tmpEB;struct Cyc_List_List*_tmp102;int _tmp103;
struct Cyc_Absyn_VarargInfo*_tmp104;struct Cyc_Core_Opt*_tmp105;struct Cyc_List_List*
_tmp106;struct Cyc_List_List*_tmp108;struct Cyc_Position_Segment*_tmp109;_LL9B: {
struct Cyc_Absyn_WithTypes_struct*_tmp101=(struct Cyc_Absyn_WithTypes_struct*)
_tmp100;if(_tmp101->tag != 1)goto _LL9D;else{_tmp102=_tmp101->f1;_tmp103=_tmp101->f2;
_tmp104=_tmp101->f3;_tmp105=_tmp101->f4;_tmp106=_tmp101->f5;}}_LL9C: {struct Cyc_PP_Doc*
_tmp6AB[2];return(_tmp6AB[1]=Cyc_Absynpp_funargs2doc(_tmp102,_tmp103,_tmp104,
_tmp105,_tmp106),((_tmp6AB[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6AB,sizeof(
struct Cyc_PP_Doc*),2)))));}_LL9D: {struct Cyc_Absyn_NoTypes_struct*_tmp107=(
struct Cyc_Absyn_NoTypes_struct*)_tmp100;if(_tmp107->tag != 0)goto _LL9A;else{
_tmp108=_tmp107->f1;_tmp109=_tmp107->f2;}}_LL9E: {const char*_tmp6B2;const char*
_tmp6B1;const char*_tmp6B0;struct Cyc_PP_Doc*_tmp6AF[2];return(_tmp6AF[1]=Cyc_PP_group(((
_tmp6B2="(",_tag_dyneither(_tmp6B2,sizeof(char),2))),((_tmp6B1=")",
_tag_dyneither(_tmp6B1,sizeof(char),2))),((_tmp6B0=",",_tag_dyneither(_tmp6B0,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp108)),((
_tmp6AF[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6AF,sizeof(struct Cyc_PP_Doc*),2)))));}
_LL9A:;}_LL94: {struct Cyc_Absyn_Attributes_mod_struct*_tmpEC=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpE4;if(_tmpEC->tag != 5)goto _LL96;else{_tmpED=_tmpEC->f2;}}_LL95: switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LL9F: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
struct Cyc_PP_Doc*_tmp6B3[2];return(_tmp6B3[1]=Cyc_Absynpp_atts2doc(_tmpED),((
_tmp6B3[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6B3,sizeof(struct Cyc_PP_Doc*),2)))));}
case Cyc_Cyclone_Vc_c: _LLA0: if(Cyc_Absynpp_next_is_pointer(tms->tl)){struct Cyc_PP_Doc*
_tmp6B4[2];return(_tmp6B4[1]=rest,((_tmp6B4[0]=Cyc_Absynpp_callconv2doc(_tmpED),
Cyc_PP_cat(_tag_dyneither(_tmp6B4,sizeof(struct Cyc_PP_Doc*),2)))));}return rest;}
_LL96: {struct Cyc_Absyn_TypeParams_mod_struct*_tmpEE=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpE4;if(_tmpEE->tag != 4)goto _LL98;else{_tmpEF=_tmpEE->f1;_tmpF0=_tmpEE->f2;
_tmpF1=_tmpEE->f3;}}_LL97: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmpF1){struct Cyc_PP_Doc*_tmp6B5[2];return(_tmp6B5[1]=Cyc_Absynpp_ktvars2doc(
_tmpEF),((_tmp6B5[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6B5,sizeof(struct Cyc_PP_Doc*),
2)))));}else{struct Cyc_PP_Doc*_tmp6B6[2];return(_tmp6B6[1]=Cyc_Absynpp_tvars2doc(
_tmpEF),((_tmp6B6[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6B6,sizeof(struct Cyc_PP_Doc*),
2)))));}_LL98: {struct Cyc_Absyn_Pointer_mod_struct*_tmpF2=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmpE4;if(_tmpF2->tag != 2)goto _LL8D;else{_tmpF3=_tmpF2->f1;_tmpF4=_tmpF3.rgn;
_tmpF5=_tmpF3.nullable;_tmpF6=_tmpF3.bounds;_tmpF7=_tmpF3.zero_term;_tmpF8=
_tmpF2->f2;}}_LL99: {struct Cyc_PP_Doc*ptr;{void*_tmp113=((void*(*)(void*y,union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmpF6);
struct Cyc_Absyn_Exp*_tmp116;_LLA3: {struct Cyc_Absyn_DynEither_b_struct*_tmp114=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp113;if(_tmp114->tag != 0)goto _LLA5;}_LLA4:
ptr=Cyc_Absynpp_question();goto _LLA2;_LLA5: {struct Cyc_Absyn_Upper_b_struct*
_tmp115=(struct Cyc_Absyn_Upper_b_struct*)_tmp113;if(_tmp115->tag != 1)goto _LLA2;
else{_tmp116=_tmp115->f1;}}_LLA6:{const char*_tmp6B8;const char*_tmp6B7;ptr=Cyc_PP_text(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmpF5)?(
_tmp6B8="*",_tag_dyneither(_tmp6B8,sizeof(char),2)):((_tmp6B7="@",_tag_dyneither(
_tmp6B7,sizeof(char),2))));}{unsigned int _tmp11A;int _tmp11B;struct _tuple10
_tmp119=Cyc_Evexp_eval_const_uint_exp(_tmp116);_tmp11A=_tmp119.f1;_tmp11B=
_tmp119.f2;if(!_tmp11B  || _tmp11A != 1){struct Cyc_PP_Doc*_tmp6B9[4];ptr=((_tmp6B9[
3]=Cyc_Absynpp_rb(),((_tmp6B9[2]=Cyc_Absynpp_exp2doc(_tmp116),((_tmp6B9[1]=Cyc_Absynpp_lb(),((
_tmp6B9[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6B9,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LLA2;}_LLA2:;}if(Cyc_Absynpp_print_zeroterm){if(!is_char_ptr  && ((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF7)){const char*
_tmp6BC;struct Cyc_PP_Doc*_tmp6BB[2];ptr=((_tmp6BB[1]=Cyc_PP_text(((_tmp6BC="@zeroterm ",
_tag_dyneither(_tmp6BC,sizeof(char),11)))),((_tmp6BB[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp6BB,sizeof(struct Cyc_PP_Doc*),2))))));}else{if(is_char_ptr
 && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF7)){
const char*_tmp6BF;struct Cyc_PP_Doc*_tmp6BE[2];ptr=((_tmp6BE[1]=Cyc_PP_text(((
_tmp6BF="@nozeroterm ",_tag_dyneither(_tmp6BF,sizeof(char),13)))),((_tmp6BE[0]=
ptr,Cyc_PP_cat(_tag_dyneither(_tmp6BE,sizeof(struct Cyc_PP_Doc*),2))))));}}}{void*
_tmp121=Cyc_Tcutil_compress(_tmpF4);_LLA8: {struct Cyc_Absyn_HeapRgn_struct*
_tmp122=(struct Cyc_Absyn_HeapRgn_struct*)_tmp121;if(_tmp122->tag != 21)goto _LLAA;}
_LLA9: goto _LLA7;_LLAA:;_LLAB: {const char*_tmp6C2;struct Cyc_PP_Doc*_tmp6C1[3];ptr=((
_tmp6C1[2]=Cyc_PP_text(((_tmp6C2=" ",_tag_dyneither(_tmp6C2,sizeof(char),2)))),((
_tmp6C1[1]=Cyc_Absynpp_typ2doc(_tmpF4),((_tmp6C1[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp6C1,sizeof(struct Cyc_PP_Doc*),3))))))));}_LLA7:;}{struct Cyc_PP_Doc*
_tmp6C3[2];ptr=((_tmp6C3[1]=Cyc_Absynpp_tqual2doc(_tmpF8),((_tmp6C3[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp6C3,sizeof(struct Cyc_PP_Doc*),2))))));}{struct Cyc_PP_Doc*
_tmp6C4[2];return(_tmp6C4[1]=rest,((_tmp6C4[0]=ptr,Cyc_PP_cat(_tag_dyneither(
_tmp6C4,sizeof(struct Cyc_PP_Doc*),2)))));}}_LL8D:;}}struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(
void*t);struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){void*_tmp12A=Cyc_Tcutil_compress(
t);_LLAD: {struct Cyc_Absyn_HeapRgn_struct*_tmp12B=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp12A;if(_tmp12B->tag != 21)goto _LLAF;}_LLAE: {const char*_tmp6C5;return Cyc_PP_text(((
_tmp6C5="`H",_tag_dyneither(_tmp6C5,sizeof(char),3))));}_LLAF: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp12C=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp12A;if(_tmp12C->tag != 22)goto
_LLB1;}_LLB0: {const char*_tmp6C6;return Cyc_PP_text(((_tmp6C6="`U",_tag_dyneither(
_tmp6C6,sizeof(char),3))));}_LLB1:;_LLB2: return Cyc_Absynpp_ntyp2doc(t);_LLAC:;}
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**
effects,void*t);static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,
struct Cyc_List_List**effects,void*t){void*_tmp12F=Cyc_Tcutil_compress(t);void*
_tmp131;struct Cyc_List_List*_tmp133;_LLB4: {struct Cyc_Absyn_AccessEff_struct*
_tmp130=(struct Cyc_Absyn_AccessEff_struct*)_tmp12F;if(_tmp130->tag != 23)goto
_LLB6;else{_tmp131=(void*)_tmp130->f1;}}_LLB5:{struct Cyc_List_List*_tmp6C7;*
rgions=((_tmp6C7=_cycalloc(sizeof(*_tmp6C7)),((_tmp6C7->hd=Cyc_Absynpp_rgn2doc(
_tmp131),((_tmp6C7->tl=*rgions,_tmp6C7))))));}goto _LLB3;_LLB6: {struct Cyc_Absyn_JoinEff_struct*
_tmp132=(struct Cyc_Absyn_JoinEff_struct*)_tmp12F;if(_tmp132->tag != 24)goto _LLB8;
else{_tmp133=_tmp132->f1;}}_LLB7: for(0;_tmp133 != 0;_tmp133=_tmp133->tl){Cyc_Absynpp_effects2docs(
rgions,effects,(void*)_tmp133->hd);}goto _LLB3;_LLB8:;_LLB9:{struct Cyc_List_List*
_tmp6C8;*effects=((_tmp6C8=_cycalloc(sizeof(*_tmp6C8)),((_tmp6C8->hd=Cyc_Absynpp_typ2doc(
t),((_tmp6C8->tl=*effects,_tmp6C8))))));}goto _LLB3;_LLB3:;}struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(
void*t);struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){struct Cyc_List_List*rgions=
0;struct Cyc_List_List*effects=0;Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
effects);if(rgions == 0  && effects != 0){const char*_tmp6CB;const char*_tmp6CA;const
char*_tmp6C9;return Cyc_PP_group(((_tmp6C9="",_tag_dyneither(_tmp6C9,sizeof(char),
1))),((_tmp6CA="",_tag_dyneither(_tmp6CA,sizeof(char),1))),((_tmp6CB="+",
_tag_dyneither(_tmp6CB,sizeof(char),2))),effects);}else{const char*_tmp6CC;struct
Cyc_PP_Doc*_tmp139=Cyc_Absynpp_group_braces(((_tmp6CC=",",_tag_dyneither(_tmp6CC,
sizeof(char),2))),rgions);struct Cyc_List_List*_tmp6D0;const char*_tmp6CF;const
char*_tmp6CE;const char*_tmp6CD;return Cyc_PP_group(((_tmp6CD="",_tag_dyneither(
_tmp6CD,sizeof(char),1))),((_tmp6CE="",_tag_dyneither(_tmp6CE,sizeof(char),1))),((
_tmp6CF="+",_tag_dyneither(_tmp6CF,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,((
_tmp6D0=_cycalloc(sizeof(*_tmp6D0)),((_tmp6D0->hd=_tmp139,((_tmp6D0->tl=0,
_tmp6D0))))))));}}struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind
k);struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){switch(k){
case Cyc_Absyn_StructA: _LLBA: {const char*_tmp6D1;return Cyc_PP_text(((_tmp6D1="struct ",
_tag_dyneither(_tmp6D1,sizeof(char),8))));}case Cyc_Absyn_UnionA: _LLBB: {const
char*_tmp6D2;return Cyc_PP_text(((_tmp6D2="union ",_tag_dyneither(_tmp6D2,sizeof(
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
_LLBF: {const char*_tmp6D3;return Cyc_PP_text(((_tmp6D3="[[[array]]]",
_tag_dyneither(_tmp6D3,sizeof(char),12))));}_LLC0: {struct Cyc_Absyn_FnType_struct*
_tmp143=(struct Cyc_Absyn_FnType_struct*)_tmp141;if(_tmp143->tag != 10)goto _LLC2;}
_LLC1: return Cyc_PP_nil_doc();_LLC2: {struct Cyc_Absyn_PointerType_struct*_tmp144=(
struct Cyc_Absyn_PointerType_struct*)_tmp141;if(_tmp144->tag != 5)goto _LLC4;}_LLC3:
return Cyc_PP_nil_doc();_LLC4: {struct Cyc_Absyn_VoidType_struct*_tmp145=(struct
Cyc_Absyn_VoidType_struct*)_tmp141;if(_tmp145->tag != 0)goto _LLC6;}_LLC5:{const
char*_tmp6D4;s=Cyc_PP_text(((_tmp6D4="void",_tag_dyneither(_tmp6D4,sizeof(char),
5))));}goto _LLBD;_LLC6: {struct Cyc_Absyn_Evar_struct*_tmp146=(struct Cyc_Absyn_Evar_struct*)
_tmp141;if(_tmp146->tag != 1)goto _LLC8;else{_tmp147=_tmp146->f1;_tmp148=_tmp146->f2;
_tmp149=_tmp146->f3;_tmp14A=_tmp146->f4;}}_LLC7: if(_tmp148 != 0)return Cyc_Absynpp_ntyp2doc((
void*)_tmp148->v);else{const char*_tmp6E4;struct Cyc_Int_pa_struct _tmp6E3;void*
_tmp6E2[1];const char*_tmp6E1;const char*_tmp6E0;const char*_tmp6DF;const char*
_tmp6DE;struct Cyc_PP_Doc*_tmp6DD[6];s=((_tmp6DD[5]=_tmp147 == 0?Cyc_Absynpp_question():
Cyc_Absynpp_kind2doc((enum Cyc_Absyn_Kind)_tmp147->v),((_tmp6DD[4]=Cyc_PP_text(((
_tmp6E4=")::",_tag_dyneither(_tmp6E4,sizeof(char),4)))),((_tmp6DD[3]=(!Cyc_Absynpp_print_full_evars
 || _tmp14A == 0)?Cyc_PP_nil_doc(): Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)
_tmp14A->v),((_tmp6DD[2]=Cyc_PP_text((struct _dyneither_ptr)((_tmp6E3.tag=1,((
_tmp6E3.f1=(unsigned long)_tmp149,((_tmp6E2[0]=& _tmp6E3,Cyc_aprintf(((_tmp6E1="%d",
_tag_dyneither(_tmp6E1,sizeof(char),3))),_tag_dyneither(_tmp6E2,sizeof(void*),1))))))))),((
_tmp6DD[1]=Cyc_PP_text(((_tmp6E0="(",_tag_dyneither(_tmp6E0,sizeof(char),2)))),((
_tmp6DD[0]=Cyc_Absynpp_print_for_cycdoc?Cyc_PP_text(((_tmp6DE="\\%",
_tag_dyneither(_tmp6DE,sizeof(char),3)))): Cyc_PP_text(((_tmp6DF="%",
_tag_dyneither(_tmp6DF,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6DD,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));}goto _LLBD;_LLC8: {struct Cyc_Absyn_VarType_struct*
_tmp14B=(struct Cyc_Absyn_VarType_struct*)_tmp141;if(_tmp14B->tag != 2)goto _LLCA;
else{_tmp14C=_tmp14B->f1;}}_LLC9: s=Cyc_PP_textptr(_tmp14C->name);if(Cyc_Absynpp_print_all_kinds){
const char*_tmp6E7;struct Cyc_PP_Doc*_tmp6E6[3];s=((_tmp6E6[2]=Cyc_Absynpp_kindbound2doc(
_tmp14C->kind),((_tmp6E6[1]=Cyc_PP_text(((_tmp6E7="::",_tag_dyneither(_tmp6E7,
sizeof(char),3)))),((_tmp6E6[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp6E6,sizeof(
struct Cyc_PP_Doc*),3))))))));}if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(
_tmp14C)){const char*_tmp6EC;const char*_tmp6EB;struct Cyc_PP_Doc*_tmp6EA[3];s=((
_tmp6EA[2]=Cyc_PP_text(((_tmp6EC=" */",_tag_dyneither(_tmp6EC,sizeof(char),4)))),((
_tmp6EA[1]=s,((_tmp6EA[0]=Cyc_PP_text(((_tmp6EB="_ /* ",_tag_dyneither(_tmp6EB,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp6EA,sizeof(struct Cyc_PP_Doc*),3))))))));}
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
_LLF6: {const char*_tmp6ED;struct Cyc_PP_Doc*_tmp193=Cyc_PP_text(((_tmp6ED="datatype ",
_tag_dyneither(_tmp6ED,sizeof(char),10))));const char*_tmp6EE;struct Cyc_PP_Doc*
_tmp194=_tmp192?Cyc_PP_text(((_tmp6EE="@extensible ",_tag_dyneither(_tmp6EE,
sizeof(char),13)))): Cyc_PP_nil_doc();{struct Cyc_PP_Doc*_tmp6EF[4];s=((_tmp6EF[3]=
Cyc_Absynpp_tps2doc(_tmp150),((_tmp6EF[2]=Cyc_Absynpp_qvar2doc(_tmp191),((
_tmp6EF[1]=_tmp193,((_tmp6EF[0]=_tmp194,Cyc_PP_cat(_tag_dyneither(_tmp6EF,
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
_tmp6F1;const char*_tmp6F0;struct Cyc_PP_Doc*_tmp1A5=Cyc_PP_text(_tmp1A1?(_tmp6F1="@extensible datatype ",
_tag_dyneither(_tmp6F1,sizeof(char),22)):((_tmp6F0="datatype ",_tag_dyneither(
_tmp6F0,sizeof(char),10))));{const char*_tmp6F4;struct Cyc_PP_Doc*_tmp6F3[4];s=((
_tmp6F3[3]=Cyc_Absynpp_qvar2doc(_tmp1A4),((_tmp6F3[2]=Cyc_PP_text(((_tmp6F4=".",
_tag_dyneither(_tmp6F4,sizeof(char),2)))),((_tmp6F3[1]=Cyc_Absynpp_qvar2doc(
_tmp1A0),((_tmp6F3[0]=_tmp1A5,Cyc_PP_cat(_tag_dyneither(_tmp6F3,sizeof(struct Cyc_PP_Doc*),
4))))))))));}goto _LLF7;}_LLF7:;}goto _LLBD;_LLCE: {struct Cyc_Absyn_IntType_struct*
_tmp155=(struct Cyc_Absyn_IntType_struct*)_tmp141;if(_tmp155->tag != 6)goto _LLD0;
else{_tmp156=_tmp155->f1;_tmp157=_tmp155->f2;}}_LLCF: {struct _dyneither_ptr sns;
struct _dyneither_ptr ts;switch(_tmp156){case Cyc_Absyn_None: _LLFC: goto _LLFD;case
Cyc_Absyn_Signed: _LLFD:{const char*_tmp6F5;sns=((_tmp6F5="",_tag_dyneither(
_tmp6F5,sizeof(char),1)));}break;case Cyc_Absyn_Unsigned: _LLFE:{const char*_tmp6F6;
sns=((_tmp6F6="unsigned ",_tag_dyneither(_tmp6F6,sizeof(char),10)));}break;}
switch(_tmp157){case Cyc_Absyn_Char_sz: _LL100: switch(_tmp156){case Cyc_Absyn_None:
_LL102:{const char*_tmp6F7;sns=((_tmp6F7="",_tag_dyneither(_tmp6F7,sizeof(char),1)));}
break;case Cyc_Absyn_Signed: _LL103:{const char*_tmp6F8;sns=((_tmp6F8="signed ",
_tag_dyneither(_tmp6F8,sizeof(char),8)));}break;case Cyc_Absyn_Unsigned: _LL104:{
const char*_tmp6F9;sns=((_tmp6F9="unsigned ",_tag_dyneither(_tmp6F9,sizeof(char),
10)));}break;}{const char*_tmp6FA;ts=((_tmp6FA="char",_tag_dyneither(_tmp6FA,
sizeof(char),5)));}break;case Cyc_Absyn_Short_sz: _LL101:{const char*_tmp6FB;ts=((
_tmp6FB="short",_tag_dyneither(_tmp6FB,sizeof(char),6)));}break;case Cyc_Absyn_Int_sz:
_LL106:{const char*_tmp6FC;ts=((_tmp6FC="int",_tag_dyneither(_tmp6FC,sizeof(char),
4)));}break;case Cyc_Absyn_Long_sz: _LL107:{const char*_tmp6FD;ts=((_tmp6FD="long",
_tag_dyneither(_tmp6FD,sizeof(char),5)));}break;case Cyc_Absyn_LongLong_sz: _LL108:
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL10A:{const char*_tmp6FE;
ts=((_tmp6FE="long long",_tag_dyneither(_tmp6FE,sizeof(char),10)));}break;case
Cyc_Cyclone_Vc_c: _LL10B:{const char*_tmp6FF;ts=((_tmp6FF="__int64",_tag_dyneither(
_tmp6FF,sizeof(char),8)));}break;}break;}{const char*_tmp704;void*_tmp703[2];
struct Cyc_String_pa_struct _tmp702;struct Cyc_String_pa_struct _tmp701;s=Cyc_PP_text((
struct _dyneither_ptr)((_tmp701.tag=0,((_tmp701.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)ts),((_tmp702.tag=0,((_tmp702.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)sns),((_tmp703[0]=& _tmp702,((_tmp703[1]=& _tmp701,Cyc_aprintf(((
_tmp704="%s%s",_tag_dyneither(_tmp704,sizeof(char),5))),_tag_dyneither(_tmp703,
sizeof(void*),2)))))))))))))));}goto _LLBD;}_LLD0: {struct Cyc_Absyn_FloatType_struct*
_tmp158=(struct Cyc_Absyn_FloatType_struct*)_tmp141;if(_tmp158->tag != 7)goto _LLD2;}
_LLD1:{const char*_tmp705;s=Cyc_PP_text(((_tmp705="float",_tag_dyneither(_tmp705,
sizeof(char),6))));}goto _LLBD;_LLD2: {struct Cyc_Absyn_DoubleType_struct*_tmp159=(
struct Cyc_Absyn_DoubleType_struct*)_tmp141;if(_tmp159->tag != 8)goto _LLD4;else{
_tmp15A=_tmp159->f1;}}_LLD3: if(_tmp15A){const char*_tmp706;s=Cyc_PP_text(((
_tmp706="long double",_tag_dyneither(_tmp706,sizeof(char),12))));}else{const char*
_tmp707;s=Cyc_PP_text(((_tmp707="double",_tag_dyneither(_tmp707,sizeof(char),7))));}
goto _LLBD;_LLD4: {struct Cyc_Absyn_TupleType_struct*_tmp15B=(struct Cyc_Absyn_TupleType_struct*)
_tmp141;if(_tmp15B->tag != 11)goto _LLD6;else{_tmp15C=_tmp15B->f1;}}_LLD5:{struct
Cyc_PP_Doc*_tmp708[2];s=((_tmp708[1]=Cyc_Absynpp_args2doc(_tmp15C),((_tmp708[0]=
Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp708,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LLBD;_LLD6: {struct Cyc_Absyn_AggrType_struct*_tmp15D=(struct Cyc_Absyn_AggrType_struct*)
_tmp141;if(_tmp15D->tag != 12)goto _LLD8;else{_tmp15E=_tmp15D->f1;_tmp15F=_tmp15E.aggr_info;
_tmp160=_tmp15E.targs;}}_LLD7: {enum Cyc_Absyn_AggrKind _tmp1BE;struct _tuple0*
_tmp1BF;struct _tuple9 _tmp1BD=Cyc_Absyn_aggr_kinded_name(_tmp15F);_tmp1BE=_tmp1BD.f1;
_tmp1BF=_tmp1BD.f2;{struct Cyc_PP_Doc*_tmp709[3];s=((_tmp709[2]=Cyc_Absynpp_tps2doc(
_tmp160),((_tmp709[1]=Cyc_Absynpp_qvar2doc(_tmp1BF),((_tmp709[0]=Cyc_Absynpp_aggr_kind2doc(
_tmp1BE),Cyc_PP_cat(_tag_dyneither(_tmp709,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLBD;}_LLD8: {struct Cyc_Absyn_AnonAggrType_struct*_tmp161=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp141;if(_tmp161->tag != 13)goto _LLDA;else{_tmp162=_tmp161->f1;_tmp163=_tmp161->f2;}}
_LLD9:{struct Cyc_PP_Doc*_tmp70A[4];s=((_tmp70A[3]=Cyc_Absynpp_rb(),((_tmp70A[2]=
Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp163)),((_tmp70A[1]=Cyc_Absynpp_lb(),((
_tmp70A[0]=Cyc_Absynpp_aggr_kind2doc(_tmp162),Cyc_PP_cat(_tag_dyneither(_tmp70A,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLBD;_LLDA: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp164=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp141;if(_tmp164->tag != 15)goto
_LLDC;else{_tmp165=_tmp164->f1;}}_LLDB:{const char*_tmp70D;struct Cyc_PP_Doc*
_tmp70C[4];s=((_tmp70C[3]=Cyc_Absynpp_rb(),((_tmp70C[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(
_tmp165)),((_tmp70C[1]=Cyc_Absynpp_lb(),((_tmp70C[0]=Cyc_PP_text(((_tmp70D="enum ",
_tag_dyneither(_tmp70D,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp70C,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLBD;_LLDC: {struct Cyc_Absyn_EnumType_struct*
_tmp166=(struct Cyc_Absyn_EnumType_struct*)_tmp141;if(_tmp166->tag != 14)goto _LLDE;
else{_tmp167=_tmp166->f1;}}_LLDD:{const char*_tmp710;struct Cyc_PP_Doc*_tmp70F[2];
s=((_tmp70F[1]=Cyc_Absynpp_qvar2doc(_tmp167),((_tmp70F[0]=Cyc_PP_text(((_tmp710="enum ",
_tag_dyneither(_tmp710,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp70F,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLBD;_LLDE: {struct Cyc_Absyn_ValueofType_struct*
_tmp168=(struct Cyc_Absyn_ValueofType_struct*)_tmp141;if(_tmp168->tag != 19)goto
_LLE0;else{_tmp169=_tmp168->f1;}}_LLDF:{const char*_tmp715;const char*_tmp714;
struct Cyc_PP_Doc*_tmp713[3];s=((_tmp713[2]=Cyc_PP_text(((_tmp715=")",
_tag_dyneither(_tmp715,sizeof(char),2)))),((_tmp713[1]=Cyc_Absynpp_exp2doc(
_tmp169),((_tmp713[0]=Cyc_PP_text(((_tmp714="valueof_t(",_tag_dyneither(_tmp714,
sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp713,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LLBD;_LLE0: {struct Cyc_Absyn_TypedefType_struct*_tmp16A=(struct
Cyc_Absyn_TypedefType_struct*)_tmp141;if(_tmp16A->tag != 18)goto _LLE2;else{
_tmp16B=_tmp16A->f1;_tmp16C=_tmp16A->f2;_tmp16D=_tmp16A->f3;}}_LLE1:{struct Cyc_PP_Doc*
_tmp716[2];s=((_tmp716[1]=Cyc_Absynpp_tps2doc(_tmp16C),((_tmp716[0]=Cyc_Absynpp_qvar2doc(
_tmp16B),Cyc_PP_cat(_tag_dyneither(_tmp716,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLBD;_LLE2: {struct Cyc_Absyn_RgnHandleType_struct*_tmp16E=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp141;if(_tmp16E->tag != 16)goto _LLE4;else{_tmp16F=(void*)_tmp16E->f1;}}_LLE3:{
const char*_tmp71B;const char*_tmp71A;struct Cyc_PP_Doc*_tmp719[3];s=((_tmp719[2]=
Cyc_PP_text(((_tmp71B=">",_tag_dyneither(_tmp71B,sizeof(char),2)))),((_tmp719[1]=
Cyc_Absynpp_rgn2doc(_tmp16F),((_tmp719[0]=Cyc_PP_text(((_tmp71A="region_t<",
_tag_dyneither(_tmp71A,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp719,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLBD;_LLE4: {struct Cyc_Absyn_DynRgnType_struct*
_tmp170=(struct Cyc_Absyn_DynRgnType_struct*)_tmp141;if(_tmp170->tag != 17)goto
_LLE6;else{_tmp171=(void*)_tmp170->f1;_tmp172=(void*)_tmp170->f2;}}_LLE5:{const
char*_tmp722;const char*_tmp721;const char*_tmp720;struct Cyc_PP_Doc*_tmp71F[5];s=((
_tmp71F[4]=Cyc_PP_text(((_tmp722=">",_tag_dyneither(_tmp722,sizeof(char),2)))),((
_tmp71F[3]=Cyc_Absynpp_rgn2doc(_tmp172),((_tmp71F[2]=Cyc_PP_text(((_tmp721=",",
_tag_dyneither(_tmp721,sizeof(char),2)))),((_tmp71F[1]=Cyc_Absynpp_rgn2doc(
_tmp171),((_tmp71F[0]=Cyc_PP_text(((_tmp720="dynregion_t<",_tag_dyneither(
_tmp720,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp71F,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LLBD;_LLE6: {struct Cyc_Absyn_TagType_struct*_tmp173=(struct
Cyc_Absyn_TagType_struct*)_tmp141;if(_tmp173->tag != 20)goto _LLE8;else{_tmp174=(
void*)_tmp173->f1;}}_LLE7:{const char*_tmp727;const char*_tmp726;struct Cyc_PP_Doc*
_tmp725[3];s=((_tmp725[2]=Cyc_PP_text(((_tmp727=">",_tag_dyneither(_tmp727,
sizeof(char),2)))),((_tmp725[1]=Cyc_Absynpp_typ2doc(_tmp174),((_tmp725[0]=Cyc_PP_text(((
_tmp726="tag_t<",_tag_dyneither(_tmp726,sizeof(char),7)))),Cyc_PP_cat(
_tag_dyneither(_tmp725,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLBD;_LLE8: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp175=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp141;if(_tmp175->tag != 22)goto _LLEA;}_LLE9: goto _LLEB;_LLEA: {struct Cyc_Absyn_HeapRgn_struct*
_tmp176=(struct Cyc_Absyn_HeapRgn_struct*)_tmp141;if(_tmp176->tag != 21)goto _LLEC;}
_LLEB: s=Cyc_Absynpp_rgn2doc(t);goto _LLBD;_LLEC: {struct Cyc_Absyn_RgnsEff_struct*
_tmp177=(struct Cyc_Absyn_RgnsEff_struct*)_tmp141;if(_tmp177->tag != 25)goto _LLEE;
else{_tmp178=(void*)_tmp177->f1;}}_LLED:{const char*_tmp72C;const char*_tmp72B;
struct Cyc_PP_Doc*_tmp72A[3];s=((_tmp72A[2]=Cyc_PP_text(((_tmp72C=")",
_tag_dyneither(_tmp72C,sizeof(char),2)))),((_tmp72A[1]=Cyc_Absynpp_typ2doc(
_tmp178),((_tmp72A[0]=Cyc_PP_text(((_tmp72B="regions(",_tag_dyneither(_tmp72B,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp72A,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLBD;_LLEE: {struct Cyc_Absyn_AccessEff_struct*_tmp179=(struct Cyc_Absyn_AccessEff_struct*)
_tmp141;if(_tmp179->tag != 23)goto _LLF0;}_LLEF: goto _LLF1;_LLF0: {struct Cyc_Absyn_JoinEff_struct*
_tmp17A=(struct Cyc_Absyn_JoinEff_struct*)_tmp141;if(_tmp17A->tag != 24)goto _LLBD;}
_LLF1: s=Cyc_Absynpp_eff2doc(t);goto _LLBD;_LLBD:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(
struct Cyc_Core_Opt*vo);struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct Cyc_Core_Opt*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*((struct _dyneither_ptr*)vo->v));}
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple11*cmp);struct Cyc_PP_Doc*
Cyc_Absynpp_rgn_cmp2doc(struct _tuple11*cmp){struct _tuple11 _tmp1D8;void*_tmp1D9;
void*_tmp1DA;struct _tuple11*_tmp1D7=cmp;_tmp1D8=*_tmp1D7;_tmp1D9=_tmp1D8.f1;
_tmp1DA=_tmp1D8.f2;{const char*_tmp72F;struct Cyc_PP_Doc*_tmp72E[3];return(_tmp72E[
2]=Cyc_Absynpp_rgn2doc(_tmp1DA),((_tmp72E[1]=Cyc_PP_text(((_tmp72F=" > ",
_tag_dyneither(_tmp72F,sizeof(char),4)))),((_tmp72E[0]=Cyc_Absynpp_rgn2doc(
_tmp1D9),Cyc_PP_cat(_tag_dyneither(_tmp72E,sizeof(struct Cyc_PP_Doc*),3)))))));}}
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po);struct Cyc_PP_Doc*
Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){const char*_tmp732;const char*
_tmp731;const char*_tmp730;return Cyc_PP_group(((_tmp730="",_tag_dyneither(_tmp730,
sizeof(char),1))),((_tmp731="",_tag_dyneither(_tmp731,sizeof(char),1))),((
_tmp732=",",_tag_dyneither(_tmp732,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,
po));}struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple7*t);struct Cyc_PP_Doc*
Cyc_Absynpp_funarg2doc(struct _tuple7*t){struct Cyc_Core_Opt*_tmp733;struct Cyc_Core_Opt*
dopt=(*t).f1 == 0?0:((_tmp733=_cycalloc(sizeof(*_tmp733)),((_tmp733->v=Cyc_PP_text(*((
struct _dyneither_ptr*)((struct Cyc_Core_Opt*)_check_null((*t).f1))->v)),_tmp733))));
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po);struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po){struct Cyc_List_List*
_tmp1E1=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple7*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);struct Cyc_PP_Doc*
eff_doc;if(c_varargs){const char*_tmp736;struct Cyc_List_List*_tmp735;_tmp1E1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp1E1,((_tmp735=_cycalloc(sizeof(*_tmp735)),((_tmp735->hd=Cyc_PP_text(((
_tmp736="...",_tag_dyneither(_tmp736,sizeof(char),4)))),((_tmp735->tl=0,_tmp735)))))));}
else{if(cyc_varargs != 0){struct _tuple7*_tmp73F;const char*_tmp73E;const char*
_tmp73D;const char*_tmp73C;struct Cyc_PP_Doc*_tmp73B[3];struct Cyc_PP_Doc*_tmp1E4=(
_tmp73B[2]=Cyc_Absynpp_funarg2doc(((_tmp73F=_cycalloc(sizeof(*_tmp73F)),((
_tmp73F->f1=cyc_varargs->name,((_tmp73F->f2=cyc_varargs->tq,((_tmp73F->f3=
cyc_varargs->type,_tmp73F))))))))),((_tmp73B[1]=cyc_varargs->inject?Cyc_PP_text(((
_tmp73D=" inject ",_tag_dyneither(_tmp73D,sizeof(char),9)))): Cyc_PP_text(((
_tmp73E=" ",_tag_dyneither(_tmp73E,sizeof(char),2)))),((_tmp73B[0]=Cyc_PP_text(((
_tmp73C="...",_tag_dyneither(_tmp73C,sizeof(char),4)))),Cyc_PP_cat(
_tag_dyneither(_tmp73B,sizeof(struct Cyc_PP_Doc*),3)))))));struct Cyc_List_List*
_tmp740;_tmp1E1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp1E1,((_tmp740=_cycalloc(sizeof(*_tmp740)),((_tmp740->hd=
_tmp1E4,((_tmp740->tl=0,_tmp740)))))));}}{const char*_tmp743;const char*_tmp742;
const char*_tmp741;struct Cyc_PP_Doc*_tmp1EB=Cyc_PP_group(((_tmp741="",
_tag_dyneither(_tmp741,sizeof(char),1))),((_tmp742="",_tag_dyneither(_tmp742,
sizeof(char),1))),((_tmp743=",",_tag_dyneither(_tmp743,sizeof(char),2))),_tmp1E1);
if(effopt != 0  && Cyc_Absynpp_print_all_effects){const char*_tmp746;struct Cyc_PP_Doc*
_tmp745[3];_tmp1EB=((_tmp745[2]=Cyc_Absynpp_eff2doc((void*)effopt->v),((_tmp745[
1]=Cyc_PP_text(((_tmp746=";",_tag_dyneither(_tmp746,sizeof(char),2)))),((_tmp745[
0]=_tmp1EB,Cyc_PP_cat(_tag_dyneither(_tmp745,sizeof(struct Cyc_PP_Doc*),3))))))));}
if(rgn_po != 0){const char*_tmp749;struct Cyc_PP_Doc*_tmp748[3];_tmp1EB=((_tmp748[2]=
Cyc_Absynpp_rgnpo2doc(rgn_po),((_tmp748[1]=Cyc_PP_text(((_tmp749=":",
_tag_dyneither(_tmp749,sizeof(char),2)))),((_tmp748[0]=_tmp1EB,Cyc_PP_cat(
_tag_dyneither(_tmp748,sizeof(struct Cyc_PP_Doc*),3))))))));}{const char*_tmp74E;
const char*_tmp74D;struct Cyc_PP_Doc*_tmp74C[3];return(_tmp74C[2]=Cyc_PP_text(((
_tmp74E=")",_tag_dyneither(_tmp74E,sizeof(char),2)))),((_tmp74C[1]=_tmp1EB,((
_tmp74C[0]=Cyc_PP_text(((_tmp74D="(",_tag_dyneither(_tmp74D,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp74C,sizeof(struct Cyc_PP_Doc*),3)))))));}}}struct
_tuple7*Cyc_Absynpp_arg_mk_opt(struct _tuple12*arg);struct _tuple7*Cyc_Absynpp_arg_mk_opt(
struct _tuple12*arg){struct Cyc_Core_Opt*_tmp751;struct _tuple7*_tmp750;return(
_tmp750=_cycalloc(sizeof(*_tmp750)),((_tmp750->f1=((_tmp751=_cycalloc(sizeof(*
_tmp751)),((_tmp751->v=(*arg).f1,_tmp751)))),((_tmp750->f2=(*arg).f2,((_tmp750->f3=(*
arg).f3,_tmp750)))))));}struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct
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
struct Cyc_List_List*_tmp752;_tmp1F8=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?(
_tmp752=_cycalloc(sizeof(*_tmp752)),((_tmp752->hd=Cyc_Absynpp_cyc_stringptr,((
_tmp752->tl=_tmp1FC,_tmp752))))): _tmp1FC;}goto _LL10D;_LL10D:;}if(Cyc_Absynpp_qvar_to_Cids){
const char*_tmp754;struct Cyc_List_List*_tmp753;return(struct _dyneither_ptr)Cyc_str_sepstr(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp1F8,((_tmp753=_cycalloc(sizeof(*_tmp753)),((_tmp753->hd=(*q).f2,((_tmp753->tl=
0,_tmp753))))))),((_tmp754="_",_tag_dyneither(_tmp754,sizeof(char),2))));}else{
if(match)return*(*q).f2;else{const char*_tmp756;struct Cyc_List_List*_tmp755;
return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1F8,((_tmp755=_cycalloc(sizeof(*
_tmp755)),((_tmp755->hd=(*q).f2,((_tmp755->tl=0,_tmp755))))))),((_tmp756="::",
_tag_dyneither(_tmp756,sizeof(char),3))));}}}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(
struct _tuple0*q);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){return
Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(
struct _tuple0*q);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _dyneither_ptr _tmp202=Cyc_Absynpp_qvar2string(q);if(Cyc_PP_tex_output){
const char*_tmp758;const char*_tmp757;return Cyc_PP_text_width((struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp757="\\textbf{",
_tag_dyneither(_tmp757,sizeof(char),9))),(struct _dyneither_ptr)_tmp202),((
_tmp758="}",_tag_dyneither(_tmp758,sizeof(char),2)))),(int)Cyc_strlen((struct
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
_LL11C: {const char*_tmp759;return(struct _dyneither_ptr)Cyc_strconcat(((_tmp759="/* bad namespace : */ ",
_tag_dyneither(_tmp759,sizeof(char),23))),(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
v));}_LL114:;}else{return*(*v).f2;}}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(
struct _tuple0*v);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(
struct _tuple0*v);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*
v){struct _dyneither_ptr _tmp20A=Cyc_Absynpp_typedef_name2string(v);if(Cyc_PP_tex_output){
const char*_tmp75B;const char*_tmp75A;return Cyc_PP_text_width((struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp75A="\\textbf{",
_tag_dyneither(_tmp75A,sizeof(char),9))),(struct _dyneither_ptr)_tmp20A),((
_tmp75B="}",_tag_dyneither(_tmp75B,sizeof(char),2)))),(int)Cyc_strlen((struct
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
_tmp247=_tmp244->f3;}}_LL18F:{const char*_tmp760;const char*_tmp75F;struct Cyc_PP_Doc*
_tmp75E[5];s=((_tmp75E[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp247),((_tmp75E[3]=
Cyc_PP_text(((_tmp760="= ",_tag_dyneither(_tmp760,sizeof(char),3)))),((_tmp75E[2]=
_tmp246 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp246->v),((
_tmp75E[1]=Cyc_PP_text(((_tmp75F=" ",_tag_dyneither(_tmp75F,sizeof(char),2)))),((
_tmp75E[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp245),Cyc_PP_cat(_tag_dyneither(
_tmp75E,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL185;_LL190: {struct Cyc_Absyn_Increment_e_struct*
_tmp248=(struct Cyc_Absyn_Increment_e_struct*)_tmp23A;if(_tmp248->tag != 5)goto
_LL192;else{_tmp249=_tmp248->f1;_tmp24A=_tmp248->f2;}}_LL191: {struct Cyc_PP_Doc*
_tmp2B6=Cyc_Absynpp_exp2doc_prec(myprec,_tmp249);switch(_tmp24A){case Cyc_Absyn_PreInc:
_LL1DA:{const char*_tmp763;struct Cyc_PP_Doc*_tmp762[2];s=((_tmp762[1]=_tmp2B6,((
_tmp762[0]=Cyc_PP_text(((_tmp763="++",_tag_dyneither(_tmp763,sizeof(char),3)))),
Cyc_PP_cat(_tag_dyneither(_tmp762,sizeof(struct Cyc_PP_Doc*),2))))));}break;case
Cyc_Absyn_PreDec: _LL1DB:{const char*_tmp766;struct Cyc_PP_Doc*_tmp765[2];s=((
_tmp765[1]=_tmp2B6,((_tmp765[0]=Cyc_PP_text(((_tmp766="--",_tag_dyneither(
_tmp766,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp765,sizeof(struct Cyc_PP_Doc*),
2))))));}break;case Cyc_Absyn_PostInc: _LL1DC:{const char*_tmp769;struct Cyc_PP_Doc*
_tmp768[2];s=((_tmp768[1]=Cyc_PP_text(((_tmp769="++",_tag_dyneither(_tmp769,
sizeof(char),3)))),((_tmp768[0]=_tmp2B6,Cyc_PP_cat(_tag_dyneither(_tmp768,
sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostDec: _LL1DD:{const char*
_tmp76C;struct Cyc_PP_Doc*_tmp76B[2];s=((_tmp76B[1]=Cyc_PP_text(((_tmp76C="--",
_tag_dyneither(_tmp76C,sizeof(char),3)))),((_tmp76B[0]=_tmp2B6,Cyc_PP_cat(
_tag_dyneither(_tmp76B,sizeof(struct Cyc_PP_Doc*),2))))));}break;}goto _LL185;}
_LL192: {struct Cyc_Absyn_Conditional_e_struct*_tmp24B=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp23A;if(_tmp24B->tag != 6)goto _LL194;else{_tmp24C=_tmp24B->f1;_tmp24D=_tmp24B->f2;
_tmp24E=_tmp24B->f3;}}_LL193:{const char*_tmp771;const char*_tmp770;struct Cyc_PP_Doc*
_tmp76F[5];s=((_tmp76F[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp24E),((_tmp76F[3]=
Cyc_PP_text(((_tmp771=" : ",_tag_dyneither(_tmp771,sizeof(char),4)))),((_tmp76F[
2]=Cyc_Absynpp_exp2doc_prec(0,_tmp24D),((_tmp76F[1]=Cyc_PP_text(((_tmp770=" ? ",
_tag_dyneither(_tmp770,sizeof(char),4)))),((_tmp76F[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp24C),Cyc_PP_cat(_tag_dyneither(_tmp76F,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL185;_LL194: {struct Cyc_Absyn_And_e_struct*_tmp24F=(struct Cyc_Absyn_And_e_struct*)
_tmp23A;if(_tmp24F->tag != 7)goto _LL196;else{_tmp250=_tmp24F->f1;_tmp251=_tmp24F->f2;}}
_LL195:{const char*_tmp774;struct Cyc_PP_Doc*_tmp773[3];s=((_tmp773[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp251),((_tmp773[1]=Cyc_PP_text(((_tmp774=" && ",_tag_dyneither(_tmp774,
sizeof(char),5)))),((_tmp773[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp250),Cyc_PP_cat(
_tag_dyneither(_tmp773,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL185;_LL196: {
struct Cyc_Absyn_Or_e_struct*_tmp252=(struct Cyc_Absyn_Or_e_struct*)_tmp23A;if(
_tmp252->tag != 8)goto _LL198;else{_tmp253=_tmp252->f1;_tmp254=_tmp252->f2;}}
_LL197:{const char*_tmp777;struct Cyc_PP_Doc*_tmp776[3];s=((_tmp776[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp254),((_tmp776[1]=Cyc_PP_text(((_tmp777=" || ",_tag_dyneither(_tmp777,
sizeof(char),5)))),((_tmp776[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp253),Cyc_PP_cat(
_tag_dyneither(_tmp776,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL185;_LL198: {
struct Cyc_Absyn_SeqExp_e_struct*_tmp255=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp23A;if(_tmp255->tag != 9)goto _LL19A;else{_tmp256=_tmp255->f1;_tmp257=_tmp255->f2;}}
_LL199:{const char*_tmp77E;const char*_tmp77D;const char*_tmp77C;struct Cyc_PP_Doc*
_tmp77B[5];s=((_tmp77B[4]=Cyc_PP_text(((_tmp77E=")",_tag_dyneither(_tmp77E,
sizeof(char),2)))),((_tmp77B[3]=Cyc_Absynpp_exp2doc(_tmp257),((_tmp77B[2]=Cyc_PP_text(((
_tmp77D=", ",_tag_dyneither(_tmp77D,sizeof(char),3)))),((_tmp77B[1]=Cyc_Absynpp_exp2doc(
_tmp256),((_tmp77B[0]=Cyc_PP_text(((_tmp77C="(",_tag_dyneither(_tmp77C,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp77B,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL185;_LL19A: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp258=(struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp23A;if(_tmp258->tag != 10)goto _LL19C;else{_tmp259=_tmp258->f1;_tmp25A=_tmp258->f2;}}
_LL19B:{const char*_tmp783;const char*_tmp782;struct Cyc_PP_Doc*_tmp781[4];s=((
_tmp781[3]=Cyc_PP_text(((_tmp783=")",_tag_dyneither(_tmp783,sizeof(char),2)))),((
_tmp781[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp25A),((_tmp781[1]=Cyc_PP_text(((
_tmp782="(",_tag_dyneither(_tmp782,sizeof(char),2)))),((_tmp781[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp259),Cyc_PP_cat(_tag_dyneither(_tmp781,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL185;_LL19C: {struct Cyc_Absyn_FnCall_e_struct*_tmp25B=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp23A;if(_tmp25B->tag != 11)goto _LL19E;else{_tmp25C=_tmp25B->f1;_tmp25D=_tmp25B->f2;}}
_LL19D:{const char*_tmp788;const char*_tmp787;struct Cyc_PP_Doc*_tmp786[4];s=((
_tmp786[3]=Cyc_PP_text(((_tmp788=")",_tag_dyneither(_tmp788,sizeof(char),2)))),((
_tmp786[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp25D),((_tmp786[1]=Cyc_PP_text(((
_tmp787="(",_tag_dyneither(_tmp787,sizeof(char),2)))),((_tmp786[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp25C),Cyc_PP_cat(_tag_dyneither(_tmp786,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL185;_LL19E: {struct Cyc_Absyn_Throw_e_struct*_tmp25E=(struct Cyc_Absyn_Throw_e_struct*)
_tmp23A;if(_tmp25E->tag != 12)goto _LL1A0;else{_tmp25F=_tmp25E->f1;}}_LL19F:{const
char*_tmp78B;struct Cyc_PP_Doc*_tmp78A[2];s=((_tmp78A[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp25F),((_tmp78A[0]=Cyc_PP_text(((_tmp78B="throw ",_tag_dyneither(
_tmp78B,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp78A,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LL185;_LL1A0: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp260=(
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp23A;if(_tmp260->tag != 13)goto _LL1A2;
else{_tmp261=_tmp260->f1;}}_LL1A1: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp261);
goto _LL185;_LL1A2: {struct Cyc_Absyn_Instantiate_e_struct*_tmp262=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp23A;if(_tmp262->tag != 14)goto _LL1A4;else{_tmp263=_tmp262->f1;}}_LL1A3: s=Cyc_Absynpp_exp2doc_prec(
inprec,_tmp263);goto _LL185;_LL1A4: {struct Cyc_Absyn_Cast_e_struct*_tmp264=(
struct Cyc_Absyn_Cast_e_struct*)_tmp23A;if(_tmp264->tag != 15)goto _LL1A6;else{
_tmp265=(void*)_tmp264->f1;_tmp266=_tmp264->f2;}}_LL1A5:{const char*_tmp790;const
char*_tmp78F;struct Cyc_PP_Doc*_tmp78E[4];s=((_tmp78E[3]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp266),((_tmp78E[2]=Cyc_PP_text(((_tmp790=")",_tag_dyneither(_tmp790,
sizeof(char),2)))),((_tmp78E[1]=Cyc_Absynpp_typ2doc(_tmp265),((_tmp78E[0]=Cyc_PP_text(((
_tmp78F="(",_tag_dyneither(_tmp78F,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp78E,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL185;_LL1A6: {struct Cyc_Absyn_Address_e_struct*
_tmp267=(struct Cyc_Absyn_Address_e_struct*)_tmp23A;if(_tmp267->tag != 16)goto
_LL1A8;else{_tmp268=_tmp267->f1;}}_LL1A7:{const char*_tmp793;struct Cyc_PP_Doc*
_tmp792[2];s=((_tmp792[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp268),((_tmp792[0]=
Cyc_PP_text(((_tmp793="&",_tag_dyneither(_tmp793,sizeof(char),2)))),Cyc_PP_cat(
_tag_dyneither(_tmp792,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL185;_LL1A8: {
struct Cyc_Absyn_New_e_struct*_tmp269=(struct Cyc_Absyn_New_e_struct*)_tmp23A;if(
_tmp269->tag != 17)goto _LL1AA;else{_tmp26A=_tmp269->f1;_tmp26B=_tmp269->f2;}}
_LL1A9: if(_tmp26A == 0){const char*_tmp796;struct Cyc_PP_Doc*_tmp795[2];s=((_tmp795[
1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp26B),((_tmp795[0]=Cyc_PP_text(((_tmp796="new ",
_tag_dyneither(_tmp796,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp795,
sizeof(struct Cyc_PP_Doc*),2))))));}else{const char*_tmp79B;const char*_tmp79A;
struct Cyc_PP_Doc*_tmp799[4];s=((_tmp799[3]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmp26B),((_tmp799[2]=Cyc_PP_text(((_tmp79B=") ",_tag_dyneither(_tmp79B,sizeof(
char),3)))),((_tmp799[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp26A),((
_tmp799[0]=Cyc_PP_text(((_tmp79A="rnew(",_tag_dyneither(_tmp79A,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmp799,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto
_LL185;_LL1AA: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp26C=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp23A;if(_tmp26C->tag != 18)goto _LL1AC;else{_tmp26D=(void*)_tmp26C->f1;}}_LL1AB:{
const char*_tmp7A0;const char*_tmp79F;struct Cyc_PP_Doc*_tmp79E[3];s=((_tmp79E[2]=
Cyc_PP_text(((_tmp7A0=")",_tag_dyneither(_tmp7A0,sizeof(char),2)))),((_tmp79E[1]=
Cyc_Absynpp_typ2doc(_tmp26D),((_tmp79E[0]=Cyc_PP_text(((_tmp79F="sizeof(",
_tag_dyneither(_tmp79F,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp79E,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL185;_LL1AC: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp26E=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp23A;if(_tmp26E->tag != 19)goto
_LL1AE;else{_tmp26F=_tmp26E->f1;}}_LL1AD:{const char*_tmp7A5;const char*_tmp7A4;
struct Cyc_PP_Doc*_tmp7A3[3];s=((_tmp7A3[2]=Cyc_PP_text(((_tmp7A5=")",
_tag_dyneither(_tmp7A5,sizeof(char),2)))),((_tmp7A3[1]=Cyc_Absynpp_exp2doc(
_tmp26F),((_tmp7A3[0]=Cyc_PP_text(((_tmp7A4="sizeof(",_tag_dyneither(_tmp7A4,
sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7A3,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL185;_LL1AE: {struct Cyc_Absyn_Valueof_e_struct*_tmp270=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp23A;if(_tmp270->tag != 40)goto _LL1B0;else{_tmp271=(void*)_tmp270->f1;}}_LL1AF:{
const char*_tmp7AA;const char*_tmp7A9;struct Cyc_PP_Doc*_tmp7A8[3];s=((_tmp7A8[2]=
Cyc_PP_text(((_tmp7AA=")",_tag_dyneither(_tmp7AA,sizeof(char),2)))),((_tmp7A8[1]=
Cyc_Absynpp_typ2doc(_tmp271),((_tmp7A8[0]=Cyc_PP_text(((_tmp7A9="valueof(",
_tag_dyneither(_tmp7A9,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7A8,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL185;_LL1B0: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp272=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp23A;if(_tmp272->tag != 39)goto
_LL1B2;else{_tmp273=_tmp272->f1;_tmp274=_tmp272->f2;}}_LL1B1:{const char*_tmp7B1;
const char*_tmp7B0;const char*_tmp7AF;struct Cyc_PP_Doc*_tmp7AE[5];s=((_tmp7AE[4]=
Cyc_PP_text(((_tmp7B1=")",_tag_dyneither(_tmp7B1,sizeof(char),2)))),((_tmp7AE[3]=
Cyc_PP_textptr(_tmp274),((_tmp7AE[2]=Cyc_PP_text(((_tmp7B0=".",_tag_dyneither(
_tmp7B0,sizeof(char),2)))),((_tmp7AE[1]=Cyc_Absynpp_exp2doc(_tmp273),((_tmp7AE[0]=
Cyc_PP_text(((_tmp7AF="tagcheck(",_tag_dyneither(_tmp7AF,sizeof(char),10)))),Cyc_PP_cat(
_tag_dyneither(_tmp7AE,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL185;
_LL1B2: {struct Cyc_Absyn_Offsetof_e_struct*_tmp275=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp23A;if(_tmp275->tag != 20)goto _LL1B4;else{_tmp276=(void*)_tmp275->f1;_tmp277=(
void*)_tmp275->f2;{struct Cyc_Absyn_StructField_struct*_tmp278=(struct Cyc_Absyn_StructField_struct*)
_tmp277;if(_tmp278->tag != 0)goto _LL1B4;else{_tmp279=_tmp278->f1;}}}}_LL1B3:{
const char*_tmp7B8;const char*_tmp7B7;const char*_tmp7B6;struct Cyc_PP_Doc*_tmp7B5[5];
s=((_tmp7B5[4]=Cyc_PP_text(((_tmp7B8=")",_tag_dyneither(_tmp7B8,sizeof(char),2)))),((
_tmp7B5[3]=Cyc_PP_textptr(_tmp279),((_tmp7B5[2]=Cyc_PP_text(((_tmp7B7=",",
_tag_dyneither(_tmp7B7,sizeof(char),2)))),((_tmp7B5[1]=Cyc_Absynpp_typ2doc(
_tmp276),((_tmp7B5[0]=Cyc_PP_text(((_tmp7B6="offsetof(",_tag_dyneither(_tmp7B6,
sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7B5,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LL185;_LL1B4: {struct Cyc_Absyn_Offsetof_e_struct*_tmp27A=(
struct Cyc_Absyn_Offsetof_e_struct*)_tmp23A;if(_tmp27A->tag != 20)goto _LL1B6;else{
_tmp27B=(void*)_tmp27A->f1;_tmp27C=(void*)_tmp27A->f2;{struct Cyc_Absyn_TupleIndex_struct*
_tmp27D=(struct Cyc_Absyn_TupleIndex_struct*)_tmp27C;if(_tmp27D->tag != 1)goto
_LL1B6;else{_tmp27E=_tmp27D->f1;}}}}_LL1B5:{const char*_tmp7C6;struct Cyc_Int_pa_struct
_tmp7C5;void*_tmp7C4[1];const char*_tmp7C3;const char*_tmp7C2;const char*_tmp7C1;
struct Cyc_PP_Doc*_tmp7C0[5];s=((_tmp7C0[4]=Cyc_PP_text(((_tmp7C6=")",
_tag_dyneither(_tmp7C6,sizeof(char),2)))),((_tmp7C0[3]=Cyc_PP_text((struct
_dyneither_ptr)((_tmp7C5.tag=1,((_tmp7C5.f1=(unsigned long)((int)_tmp27E),((
_tmp7C4[0]=& _tmp7C5,Cyc_aprintf(((_tmp7C3="%d",_tag_dyneither(_tmp7C3,sizeof(
char),3))),_tag_dyneither(_tmp7C4,sizeof(void*),1))))))))),((_tmp7C0[2]=Cyc_PP_text(((
_tmp7C2=",",_tag_dyneither(_tmp7C2,sizeof(char),2)))),((_tmp7C0[1]=Cyc_Absynpp_typ2doc(
_tmp27B),((_tmp7C0[0]=Cyc_PP_text(((_tmp7C1="offsetof(",_tag_dyneither(_tmp7C1,
sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7C0,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LL185;_LL1B6: {struct Cyc_Absyn_Gentyp_e_struct*_tmp27F=(
struct Cyc_Absyn_Gentyp_e_struct*)_tmp23A;if(_tmp27F->tag != 21)goto _LL1B8;else{
_tmp280=_tmp27F->f1;_tmp281=(void*)_tmp27F->f2;}}_LL1B7:{const char*_tmp7CB;const
char*_tmp7CA;struct Cyc_PP_Doc*_tmp7C9[4];s=((_tmp7C9[3]=Cyc_PP_text(((_tmp7CB=")",
_tag_dyneither(_tmp7CB,sizeof(char),2)))),((_tmp7C9[2]=Cyc_Absynpp_typ2doc(
_tmp281),((_tmp7C9[1]=Cyc_Absynpp_tvars2doc(_tmp280),((_tmp7C9[0]=Cyc_PP_text(((
_tmp7CA="__gen(",_tag_dyneither(_tmp7CA,sizeof(char),7)))),Cyc_PP_cat(
_tag_dyneither(_tmp7C9,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL185;_LL1B8: {
struct Cyc_Absyn_Deref_e_struct*_tmp282=(struct Cyc_Absyn_Deref_e_struct*)_tmp23A;
if(_tmp282->tag != 22)goto _LL1BA;else{_tmp283=_tmp282->f1;}}_LL1B9:{const char*
_tmp7CE;struct Cyc_PP_Doc*_tmp7CD[2];s=((_tmp7CD[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp283),((_tmp7CD[0]=Cyc_PP_text(((_tmp7CE="*",_tag_dyneither(_tmp7CE,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7CD,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL185;_LL1BA: {struct Cyc_Absyn_AggrMember_e_struct*_tmp284=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp23A;if(_tmp284->tag != 23)goto _LL1BC;else{_tmp285=_tmp284->f1;_tmp286=_tmp284->f2;}}
_LL1BB:{const char*_tmp7D1;struct Cyc_PP_Doc*_tmp7D0[3];s=((_tmp7D0[2]=Cyc_PP_textptr(
_tmp286),((_tmp7D0[1]=Cyc_PP_text(((_tmp7D1=".",_tag_dyneither(_tmp7D1,sizeof(
char),2)))),((_tmp7D0[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp285),Cyc_PP_cat(
_tag_dyneither(_tmp7D0,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL185;_LL1BC: {
struct Cyc_Absyn_AggrArrow_e_struct*_tmp287=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp23A;if(_tmp287->tag != 24)goto _LL1BE;else{_tmp288=_tmp287->f1;_tmp289=_tmp287->f2;}}
_LL1BD:{const char*_tmp7D4;struct Cyc_PP_Doc*_tmp7D3[3];s=((_tmp7D3[2]=Cyc_PP_textptr(
_tmp289),((_tmp7D3[1]=Cyc_PP_text(((_tmp7D4="->",_tag_dyneither(_tmp7D4,sizeof(
char),3)))),((_tmp7D3[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp288),Cyc_PP_cat(
_tag_dyneither(_tmp7D3,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL185;_LL1BE: {
struct Cyc_Absyn_Subscript_e_struct*_tmp28A=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp23A;if(_tmp28A->tag != 25)goto _LL1C0;else{_tmp28B=_tmp28A->f1;_tmp28C=_tmp28A->f2;}}
_LL1BF:{const char*_tmp7D9;const char*_tmp7D8;struct Cyc_PP_Doc*_tmp7D7[4];s=((
_tmp7D7[3]=Cyc_PP_text(((_tmp7D9="]",_tag_dyneither(_tmp7D9,sizeof(char),2)))),((
_tmp7D7[2]=Cyc_Absynpp_exp2doc(_tmp28C),((_tmp7D7[1]=Cyc_PP_text(((_tmp7D8="[",
_tag_dyneither(_tmp7D8,sizeof(char),2)))),((_tmp7D7[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp28B),Cyc_PP_cat(_tag_dyneither(_tmp7D7,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL185;_LL1C0: {struct Cyc_Absyn_Tuple_e_struct*_tmp28D=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp23A;if(_tmp28D->tag != 26)goto _LL1C2;else{_tmp28E=_tmp28D->f1;}}_LL1C1:{const
char*_tmp7DE;const char*_tmp7DD;struct Cyc_PP_Doc*_tmp7DC[4];s=((_tmp7DC[3]=Cyc_PP_text(((
_tmp7DE=")",_tag_dyneither(_tmp7DE,sizeof(char),2)))),((_tmp7DC[2]=Cyc_Absynpp_exps2doc_prec(
20,_tmp28E),((_tmp7DC[1]=Cyc_PP_text(((_tmp7DD="(",_tag_dyneither(_tmp7DD,
sizeof(char),2)))),((_tmp7DC[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(
_tmp7DC,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL185;_LL1C2: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp28F=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp23A;if(_tmp28F->tag != 27)goto
_LL1C4;else{_tmp290=_tmp28F->f1;_tmp291=_tmp28F->f2;}}_LL1C3:{const char*_tmp7E5;
const char*_tmp7E4;const char*_tmp7E3;struct Cyc_PP_Doc*_tmp7E2[4];s=((_tmp7E2[3]=
Cyc_Absynpp_group_braces(((_tmp7E5=",",_tag_dyneither(_tmp7E5,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp291)),((_tmp7E2[2]=Cyc_PP_text(((_tmp7E4=")",
_tag_dyneither(_tmp7E4,sizeof(char),2)))),((_tmp7E2[1]=Cyc_Absynpp_typ2doc((*
_tmp290).f3),((_tmp7E2[0]=Cyc_PP_text(((_tmp7E3="(",_tag_dyneither(_tmp7E3,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7E2,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL185;_LL1C4: {struct Cyc_Absyn_Array_e_struct*_tmp292=(struct Cyc_Absyn_Array_e_struct*)
_tmp23A;if(_tmp292->tag != 28)goto _LL1C6;else{_tmp293=_tmp292->f1;}}_LL1C5:{const
char*_tmp7E6;s=Cyc_Absynpp_group_braces(((_tmp7E6=",",_tag_dyneither(_tmp7E6,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp293));}goto _LL185;
_LL1C6: {struct Cyc_Absyn_Comprehension_e_struct*_tmp294=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp23A;if(_tmp294->tag != 29)goto _LL1C8;else{_tmp295=_tmp294->f1;_tmp296=_tmp294->f2;
_tmp297=_tmp294->f3;}}_LL1C7:{const char*_tmp7ED;const char*_tmp7EC;const char*
_tmp7EB;struct Cyc_PP_Doc*_tmp7EA[8];s=((_tmp7EA[7]=Cyc_Absynpp_rb(),((_tmp7EA[6]=
Cyc_Absynpp_exp2doc(_tmp297),((_tmp7EA[5]=Cyc_PP_text(((_tmp7ED=" : ",
_tag_dyneither(_tmp7ED,sizeof(char),4)))),((_tmp7EA[4]=Cyc_Absynpp_exp2doc(
_tmp296),((_tmp7EA[3]=Cyc_PP_text(((_tmp7EC=" < ",_tag_dyneither(_tmp7EC,sizeof(
char),4)))),((_tmp7EA[2]=Cyc_PP_text(*(*_tmp295->name).f2),((_tmp7EA[1]=Cyc_PP_text(((
_tmp7EB="for ",_tag_dyneither(_tmp7EB,sizeof(char),5)))),((_tmp7EA[0]=Cyc_Absynpp_lb(),
Cyc_PP_cat(_tag_dyneither(_tmp7EA,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL185;_LL1C8: {struct Cyc_Absyn_Aggregate_e_struct*_tmp298=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp23A;if(_tmp298->tag != 30)goto _LL1CA;else{_tmp299=_tmp298->f1;_tmp29A=_tmp298->f2;
_tmp29B=_tmp298->f3;}}_LL1C9: {struct Cyc_List_List*_tmp30C=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp29B);{const char*_tmp7F2;struct Cyc_List_List*_tmp7F1;struct Cyc_PP_Doc*_tmp7F0[
2];s=((_tmp7F0[1]=Cyc_Absynpp_group_braces(((_tmp7F2=",",_tag_dyneither(_tmp7F2,
sizeof(char),2))),_tmp29A != 0?(_tmp7F1=_cycalloc(sizeof(*_tmp7F1)),((_tmp7F1->hd=
Cyc_Absynpp_tps2doc(_tmp29A),((_tmp7F1->tl=_tmp30C,_tmp7F1))))): _tmp30C),((
_tmp7F0[0]=Cyc_Absynpp_qvar2doc(_tmp299),Cyc_PP_cat(_tag_dyneither(_tmp7F0,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL185;}_LL1CA: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp29C=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp23A;if(_tmp29C->tag != 31)goto
_LL1CC;else{_tmp29D=_tmp29C->f2;}}_LL1CB:{const char*_tmp7F3;s=Cyc_Absynpp_group_braces(((
_tmp7F3=",",_tag_dyneither(_tmp7F3,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp29D));}goto _LL185;_LL1CC: {struct Cyc_Absyn_Datatype_e_struct*_tmp29E=(struct
Cyc_Absyn_Datatype_e_struct*)_tmp23A;if(_tmp29E->tag != 32)goto _LL1CE;else{
_tmp29F=_tmp29E->f1;_tmp2A0=_tmp29E->f3;}}_LL1CD: if(_tmp29F == 0)s=Cyc_Absynpp_qvar2doc(
_tmp2A0->name);else{const char*_tmp7FA;const char*_tmp7F9;const char*_tmp7F8;struct
Cyc_PP_Doc*_tmp7F7[2];s=((_tmp7F7[1]=Cyc_PP_egroup(((_tmp7FA="(",_tag_dyneither(
_tmp7FA,sizeof(char),2))),((_tmp7F9=")",_tag_dyneither(_tmp7F9,sizeof(char),2))),((
_tmp7F8=",",_tag_dyneither(_tmp7F8,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_exp2doc,_tmp29F)),((_tmp7F7[0]=Cyc_Absynpp_qvar2doc(_tmp2A0->name),
Cyc_PP_cat(_tag_dyneither(_tmp7F7,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL185;
_LL1CE: {struct Cyc_Absyn_Enum_e_struct*_tmp2A1=(struct Cyc_Absyn_Enum_e_struct*)
_tmp23A;if(_tmp2A1->tag != 33)goto _LL1D0;else{_tmp2A2=_tmp2A1->f1;}}_LL1CF: s=Cyc_Absynpp_qvar2doc(
_tmp2A2);goto _LL185;_LL1D0: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp2A3=(struct
Cyc_Absyn_AnonEnum_e_struct*)_tmp23A;if(_tmp2A3->tag != 34)goto _LL1D2;else{
_tmp2A4=_tmp2A3->f1;}}_LL1D1: s=Cyc_Absynpp_qvar2doc(_tmp2A4);goto _LL185;_LL1D2: {
struct Cyc_Absyn_Malloc_e_struct*_tmp2A5=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp23A;if(_tmp2A5->tag != 35)goto _LL1D4;else{_tmp2A6=_tmp2A5->f1;_tmp2A7=_tmp2A6.is_calloc;
_tmp2A8=_tmp2A6.rgn;_tmp2A9=_tmp2A6.elt_type;_tmp2AA=_tmp2A6.num_elts;}}_LL1D3:
if(_tmp2A7){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(
_tmp2A9)),0);if(_tmp2A8 == 0){const char*_tmp801;const char*_tmp800;const char*
_tmp7FF;struct Cyc_PP_Doc*_tmp7FE[5];s=((_tmp7FE[4]=Cyc_PP_text(((_tmp801=")",
_tag_dyneither(_tmp801,sizeof(char),2)))),((_tmp7FE[3]=Cyc_Absynpp_exp2doc(st),((
_tmp7FE[2]=Cyc_PP_text(((_tmp800=",",_tag_dyneither(_tmp800,sizeof(char),2)))),((
_tmp7FE[1]=Cyc_Absynpp_exp2doc(_tmp2AA),((_tmp7FE[0]=Cyc_PP_text(((_tmp7FF="calloc(",
_tag_dyneither(_tmp7FF,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7FE,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{const char*_tmp80A;const char*
_tmp809;const char*_tmp808;const char*_tmp807;struct Cyc_PP_Doc*_tmp806[7];s=((
_tmp806[6]=Cyc_PP_text(((_tmp80A=")",_tag_dyneither(_tmp80A,sizeof(char),2)))),((
_tmp806[5]=Cyc_Absynpp_exp2doc(st),((_tmp806[4]=Cyc_PP_text(((_tmp809=",",
_tag_dyneither(_tmp809,sizeof(char),2)))),((_tmp806[3]=Cyc_Absynpp_exp2doc(
_tmp2AA),((_tmp806[2]=Cyc_PP_text(((_tmp808=",",_tag_dyneither(_tmp808,sizeof(
char),2)))),((_tmp806[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp2A8),((
_tmp806[0]=Cyc_PP_text(((_tmp807="rcalloc(",_tag_dyneither(_tmp807,sizeof(char),
9)))),Cyc_PP_cat(_tag_dyneither(_tmp806,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}
else{struct Cyc_Absyn_Exp*new_e;if(_tmp2A9 == 0)new_e=_tmp2AA;else{new_e=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(*_tmp2A9,0),_tmp2AA,0);}if(_tmp2A8 == 0){const char*
_tmp80F;const char*_tmp80E;struct Cyc_PP_Doc*_tmp80D[3];s=((_tmp80D[2]=Cyc_PP_text(((
_tmp80F=")",_tag_dyneither(_tmp80F,sizeof(char),2)))),((_tmp80D[1]=Cyc_Absynpp_exp2doc(
new_e),((_tmp80D[0]=Cyc_PP_text(((_tmp80E="malloc(",_tag_dyneither(_tmp80E,
sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp80D,sizeof(struct Cyc_PP_Doc*),3))))))));}
else{const char*_tmp816;const char*_tmp815;const char*_tmp814;struct Cyc_PP_Doc*
_tmp813[5];s=((_tmp813[4]=Cyc_PP_text(((_tmp816=")",_tag_dyneither(_tmp816,
sizeof(char),2)))),((_tmp813[3]=Cyc_Absynpp_exp2doc(new_e),((_tmp813[2]=Cyc_PP_text(((
_tmp815=",",_tag_dyneither(_tmp815,sizeof(char),2)))),((_tmp813[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp2A8),((_tmp813[0]=Cyc_PP_text(((_tmp814="rmalloc(",
_tag_dyneither(_tmp814,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp813,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}}goto _LL185;_LL1D4: {struct Cyc_Absyn_Swap_e_struct*
_tmp2AB=(struct Cyc_Absyn_Swap_e_struct*)_tmp23A;if(_tmp2AB->tag != 36)goto _LL1D6;
else{_tmp2AC=_tmp2AB->f1;_tmp2AD=_tmp2AB->f2;}}_LL1D5:{const char*_tmp81D;const
char*_tmp81C;const char*_tmp81B;struct Cyc_PP_Doc*_tmp81A[5];s=((_tmp81A[4]=Cyc_PP_text(((
_tmp81D=")",_tag_dyneither(_tmp81D,sizeof(char),2)))),((_tmp81A[3]=Cyc_Absynpp_exp2doc(
_tmp2AD),((_tmp81A[2]=Cyc_PP_text(((_tmp81C=",",_tag_dyneither(_tmp81C,sizeof(
char),2)))),((_tmp81A[1]=Cyc_Absynpp_exp2doc(_tmp2AC),((_tmp81A[0]=Cyc_PP_text(((
_tmp81B="cycswap(",_tag_dyneither(_tmp81B,sizeof(char),9)))),Cyc_PP_cat(
_tag_dyneither(_tmp81A,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL185;
_LL1D6: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp2AE=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp23A;if(_tmp2AE->tag != 37)goto _LL1D8;else{_tmp2AF=_tmp2AE->f1;_tmp2B0=_tmp2AE->f2;}}
_LL1D7:{const char*_tmp81E;s=Cyc_Absynpp_group_braces(((_tmp81E=",",
_tag_dyneither(_tmp81E,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp2B0));}goto _LL185;_LL1D8: {struct Cyc_Absyn_StmtExp_e_struct*_tmp2B1=(struct
Cyc_Absyn_StmtExp_e_struct*)_tmp23A;if(_tmp2B1->tag != 38)goto _LL185;else{_tmp2B2=
_tmp2B1->f1;}}_LL1D9:{const char*_tmp823;const char*_tmp822;struct Cyc_PP_Doc*
_tmp821[7];s=((_tmp821[6]=Cyc_PP_text(((_tmp823=")",_tag_dyneither(_tmp823,
sizeof(char),2)))),((_tmp821[5]=Cyc_Absynpp_rb(),((_tmp821[4]=Cyc_PP_blank_doc(),((
_tmp821[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2B2)),((_tmp821[2]=Cyc_PP_blank_doc(),((
_tmp821[1]=Cyc_Absynpp_lb(),((_tmp821[0]=Cyc_PP_text(((_tmp822="(",
_tag_dyneither(_tmp822,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp821,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}goto _LL185;_LL185:;}if(inprec >= 
myprec){const char*_tmp828;const char*_tmp827;struct Cyc_PP_Doc*_tmp826[3];s=((
_tmp826[2]=Cyc_PP_text(((_tmp828=")",_tag_dyneither(_tmp828,sizeof(char),2)))),((
_tmp826[1]=s,((_tmp826[0]=Cyc_PP_text(((_tmp827="(",_tag_dyneither(_tmp827,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp826,sizeof(struct Cyc_PP_Doc*),3))))))));}
return s;}struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d);struct Cyc_PP_Doc*
Cyc_Absynpp_designator2doc(void*d){void*_tmp330=d;struct Cyc_Absyn_Exp*_tmp332;
struct _dyneither_ptr*_tmp334;_LL1E0: {struct Cyc_Absyn_ArrayElement_struct*
_tmp331=(struct Cyc_Absyn_ArrayElement_struct*)_tmp330;if(_tmp331->tag != 0)goto
_LL1E2;else{_tmp332=_tmp331->f1;}}_LL1E1: {const char*_tmp82D;const char*_tmp82C;
struct Cyc_PP_Doc*_tmp82B[3];return(_tmp82B[2]=Cyc_PP_text(((_tmp82D="]",
_tag_dyneither(_tmp82D,sizeof(char),2)))),((_tmp82B[1]=Cyc_Absynpp_exp2doc(
_tmp332),((_tmp82B[0]=Cyc_PP_text(((_tmp82C=".[",_tag_dyneither(_tmp82C,sizeof(
char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp82B,sizeof(struct Cyc_PP_Doc*),3)))))));}
_LL1E2: {struct Cyc_Absyn_FieldName_struct*_tmp333=(struct Cyc_Absyn_FieldName_struct*)
_tmp330;if(_tmp333->tag != 1)goto _LL1DF;else{_tmp334=_tmp333->f1;}}_LL1E3: {const
char*_tmp830;struct Cyc_PP_Doc*_tmp82F[2];return(_tmp82F[1]=Cyc_PP_textptr(
_tmp334),((_tmp82F[0]=Cyc_PP_text(((_tmp830=".",_tag_dyneither(_tmp830,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp82F,sizeof(struct Cyc_PP_Doc*),2)))));}
_LL1DF:;}struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple15*de);struct Cyc_PP_Doc*
Cyc_Absynpp_de2doc(struct _tuple15*de){if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*
de).f2);else{const char*_tmp837;const char*_tmp836;const char*_tmp835;struct Cyc_PP_Doc*
_tmp834[2];return(_tmp834[1]=Cyc_Absynpp_exp2doc((*de).f2),((_tmp834[0]=Cyc_PP_egroup(((
_tmp837="",_tag_dyneither(_tmp837,sizeof(char),1))),((_tmp836="=",_tag_dyneither(
_tmp836,sizeof(char),2))),((_tmp835="=",_tag_dyneither(_tmp835,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*de).f1)),Cyc_PP_cat(_tag_dyneither(_tmp834,sizeof(
struct Cyc_PP_Doc*),2)))));}}struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,
struct Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,
struct Cyc_List_List*es){const char*_tmp83A;const char*_tmp839;const char*_tmp838;
return Cyc_PP_group(((_tmp838="",_tag_dyneither(_tmp838,sizeof(char),1))),((
_tmp839="",_tag_dyneither(_tmp839,sizeof(char),1))),((_tmp83A=",",_tag_dyneither(
_tmp83A,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,
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
_tmp342.f2;_LL1E6: {const char*_tmp83E;void*_tmp83D[1];struct Cyc_String_pa_struct
_tmp83C;return Cyc_PP_text((struct _dyneither_ptr)((_tmp83C.tag=0,((_tmp83C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp344)),((
_tmp83D[0]=& _tmp83C,Cyc_aprintf(((_tmp83E="'%s'",_tag_dyneither(_tmp83E,sizeof(
char),5))),_tag_dyneither(_tmp83D,sizeof(void*),1)))))))));}_LL1E7: if((_tmp341.Short_c).tag
!= 3)goto _LL1E9;_tmp345=(struct _tuple4)(_tmp341.Short_c).val;_tmp346=_tmp345.f1;
_tmp347=_tmp345.f2;_LL1E8: {const char*_tmp842;void*_tmp841[1];struct Cyc_Int_pa_struct
_tmp840;return Cyc_PP_text((struct _dyneither_ptr)((_tmp840.tag=1,((_tmp840.f1=(
unsigned long)((int)_tmp347),((_tmp841[0]=& _tmp840,Cyc_aprintf(((_tmp842="%d",
_tag_dyneither(_tmp842,sizeof(char),3))),_tag_dyneither(_tmp841,sizeof(void*),1)))))))));}
_LL1E9: if((_tmp341.Int_c).tag != 4)goto _LL1EB;_tmp348=(struct _tuple5)(_tmp341.Int_c).val;
_tmp349=_tmp348.f1;if(_tmp349 != Cyc_Absyn_None)goto _LL1EB;_tmp34A=_tmp348.f2;
_LL1EA: _tmp34D=_tmp34A;goto _LL1EC;_LL1EB: if((_tmp341.Int_c).tag != 4)goto _LL1ED;
_tmp34B=(struct _tuple5)(_tmp341.Int_c).val;_tmp34C=_tmp34B.f1;if(_tmp34C != Cyc_Absyn_Signed)
goto _LL1ED;_tmp34D=_tmp34B.f2;_LL1EC: {const char*_tmp846;void*_tmp845[1];struct
Cyc_Int_pa_struct _tmp844;return Cyc_PP_text((struct _dyneither_ptr)((_tmp844.tag=1,((
_tmp844.f1=(unsigned long)_tmp34D,((_tmp845[0]=& _tmp844,Cyc_aprintf(((_tmp846="%d",
_tag_dyneither(_tmp846,sizeof(char),3))),_tag_dyneither(_tmp845,sizeof(void*),1)))))))));}
_LL1ED: if((_tmp341.Int_c).tag != 4)goto _LL1EF;_tmp34E=(struct _tuple5)(_tmp341.Int_c).val;
_tmp34F=_tmp34E.f1;if(_tmp34F != Cyc_Absyn_Unsigned)goto _LL1EF;_tmp350=_tmp34E.f2;
_LL1EE: {const char*_tmp84A;void*_tmp849[1];struct Cyc_Int_pa_struct _tmp848;return
Cyc_PP_text((struct _dyneither_ptr)((_tmp848.tag=1,((_tmp848.f1=(unsigned int)
_tmp350,((_tmp849[0]=& _tmp848,Cyc_aprintf(((_tmp84A="%u",_tag_dyneither(_tmp84A,
sizeof(char),3))),_tag_dyneither(_tmp849,sizeof(void*),1)))))))));}_LL1EF: if((
_tmp341.LongLong_c).tag != 5)goto _LL1F1;_tmp351=(struct _tuple6)(_tmp341.LongLong_c).val;
_tmp352=_tmp351.f1;_tmp353=_tmp351.f2;_LL1F0: {const char*_tmp84B;return Cyc_PP_text(((
_tmp84B="<<FIX LONG LONG CONSTANT>>",_tag_dyneither(_tmp84B,sizeof(char),27))));}
_LL1F1: if((_tmp341.Float_c).tag != 6)goto _LL1F3;_tmp354=(struct _dyneither_ptr)(
_tmp341.Float_c).val;_LL1F2: return Cyc_PP_text(_tmp354);_LL1F3: if((_tmp341.Null_c).tag
!= 1)goto _LL1F5;_tmp355=(int)(_tmp341.Null_c).val;_LL1F4: {const char*_tmp84C;
return Cyc_PP_text(((_tmp84C="NULL",_tag_dyneither(_tmp84C,sizeof(char),5))));}
_LL1F5: if((_tmp341.String_c).tag != 7)goto _LL1E4;_tmp356=(struct _dyneither_ptr)(
_tmp341.String_c).val;_LL1F6: {const char*_tmp851;const char*_tmp850;struct Cyc_PP_Doc*
_tmp84F[3];return(_tmp84F[2]=Cyc_PP_text(((_tmp851="\"",_tag_dyneither(_tmp851,
sizeof(char),2)))),((_tmp84F[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp356)),((
_tmp84F[0]=Cyc_PP_text(((_tmp850="\"",_tag_dyneither(_tmp850,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp84F,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL1E4:;}
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct
Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop
p,struct Cyc_List_List*es){struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);if(p == (
enum Cyc_Absyn_Primop)Cyc_Absyn_Numelts){if(es == 0  || es->tl != 0){struct Cyc_Core_Failure_struct
_tmp85E;const char*_tmp85D;void*_tmp85C[1];struct Cyc_String_pa_struct _tmp85B;
struct Cyc_Core_Failure_struct*_tmp85A;(int)_throw((void*)((_tmp85A=_cycalloc(
sizeof(*_tmp85A)),((_tmp85A[0]=((_tmp85E.tag=Cyc_Core_Failure,((_tmp85E.f1=(
struct _dyneither_ptr)((_tmp85B.tag=0,((_tmp85B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp85C[0]=& _tmp85B,Cyc_aprintf(((
_tmp85D="Absynpp::primapp2doc Numelts: %s with bad args",_tag_dyneither(_tmp85D,
sizeof(char),47))),_tag_dyneither(_tmp85C,sizeof(void*),1)))))))),_tmp85E)))),
_tmp85A)))));}{const char*_tmp863;const char*_tmp862;struct Cyc_PP_Doc*_tmp861[3];
return(_tmp861[2]=Cyc_PP_text(((_tmp863=")",_tag_dyneither(_tmp863,sizeof(char),
2)))),((_tmp861[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd),((_tmp861[0]=
Cyc_PP_text(((_tmp862="numelts(",_tag_dyneither(_tmp862,sizeof(char),9)))),Cyc_PP_cat(
_tag_dyneither(_tmp861,sizeof(struct Cyc_PP_Doc*),3)))))));}}else{struct Cyc_List_List*
ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0){struct Cyc_Core_Failure_struct _tmp870;const char*_tmp86F;void*_tmp86E[1];
struct Cyc_String_pa_struct _tmp86D;struct Cyc_Core_Failure_struct*_tmp86C;(int)
_throw((void*)((_tmp86C=_cycalloc(sizeof(*_tmp86C)),((_tmp86C[0]=((_tmp870.tag=
Cyc_Core_Failure,((_tmp870.f1=(struct _dyneither_ptr)((_tmp86D.tag=0,((_tmp86D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((
_tmp86E[0]=& _tmp86D,Cyc_aprintf(((_tmp86F="Absynpp::primapp2doc: %s with no args",
_tag_dyneither(_tmp86F,sizeof(char),38))),_tag_dyneither(_tmp86E,sizeof(void*),1)))))))),
_tmp870)))),_tmp86C)))));}else{if(ds->tl == 0){const char*_tmp873;struct Cyc_PP_Doc*
_tmp872[3];return(_tmp872[2]=(struct Cyc_PP_Doc*)ds->hd,((_tmp872[1]=Cyc_PP_text(((
_tmp873=" ",_tag_dyneither(_tmp873,sizeof(char),2)))),((_tmp872[0]=ps,Cyc_PP_cat(
_tag_dyneither(_tmp872,sizeof(struct Cyc_PP_Doc*),3)))))));}else{if(((struct Cyc_List_List*)
_check_null(ds->tl))->tl != 0){struct Cyc_Core_Failure_struct _tmp880;const char*
_tmp87F;void*_tmp87E[1];struct Cyc_String_pa_struct _tmp87D;struct Cyc_Core_Failure_struct*
_tmp87C;(int)_throw((void*)((_tmp87C=_cycalloc(sizeof(*_tmp87C)),((_tmp87C[0]=((
_tmp880.tag=Cyc_Core_Failure,((_tmp880.f1=(struct _dyneither_ptr)((_tmp87D.tag=0,((
_tmp87D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,
72)),((_tmp87E[0]=& _tmp87D,Cyc_aprintf(((_tmp87F="Absynpp::primapp2doc: %s with more than 2 args",
_tag_dyneither(_tmp87F,sizeof(char),47))),_tag_dyneither(_tmp87E,sizeof(void*),1)))))))),
_tmp880)))),_tmp87C)))));}else{const char*_tmp885;const char*_tmp884;struct Cyc_PP_Doc*
_tmp883[5];return(_tmp883[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)
_check_null(ds->tl))->hd,((_tmp883[3]=Cyc_PP_text(((_tmp885=" ",_tag_dyneither(
_tmp885,sizeof(char),2)))),((_tmp883[2]=ps,((_tmp883[1]=Cyc_PP_text(((_tmp884=" ",
_tag_dyneither(_tmp884,sizeof(char),2)))),((_tmp883[0]=(struct Cyc_PP_Doc*)ds->hd,
Cyc_PP_cat(_tag_dyneither(_tmp883,sizeof(struct Cyc_PP_Doc*),5)))))))))));}}}}}
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p);struct
_dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){switch(p){case Cyc_Absyn_Plus:
_LL1F7: {const char*_tmp886;return(_tmp886="+",_tag_dyneither(_tmp886,sizeof(char),
2));}case Cyc_Absyn_Times: _LL1F8: {const char*_tmp887;return(_tmp887="*",
_tag_dyneither(_tmp887,sizeof(char),2));}case Cyc_Absyn_Minus: _LL1F9: {const char*
_tmp888;return(_tmp888="-",_tag_dyneither(_tmp888,sizeof(char),2));}case Cyc_Absyn_Div:
_LL1FA: {const char*_tmp889;return(_tmp889="/",_tag_dyneither(_tmp889,sizeof(char),
2));}case Cyc_Absyn_Mod: _LL1FB: {const char*_tmp88B;const char*_tmp88A;return Cyc_Absynpp_print_for_cycdoc?(
_tmp88B="\\%",_tag_dyneither(_tmp88B,sizeof(char),3)):((_tmp88A="%",
_tag_dyneither(_tmp88A,sizeof(char),2)));}case Cyc_Absyn_Eq: _LL1FC: {const char*
_tmp88C;return(_tmp88C="==",_tag_dyneither(_tmp88C,sizeof(char),3));}case Cyc_Absyn_Neq:
_LL1FD: {const char*_tmp88D;return(_tmp88D="!=",_tag_dyneither(_tmp88D,sizeof(
char),3));}case Cyc_Absyn_Gt: _LL1FE: {const char*_tmp88E;return(_tmp88E=">",
_tag_dyneither(_tmp88E,sizeof(char),2));}case Cyc_Absyn_Lt: _LL1FF: {const char*
_tmp88F;return(_tmp88F="<",_tag_dyneither(_tmp88F,sizeof(char),2));}case Cyc_Absyn_Gte:
_LL200: {const char*_tmp890;return(_tmp890=">=",_tag_dyneither(_tmp890,sizeof(
char),3));}case Cyc_Absyn_Lte: _LL201: {const char*_tmp891;return(_tmp891="<=",
_tag_dyneither(_tmp891,sizeof(char),3));}case Cyc_Absyn_Not: _LL202: {const char*
_tmp892;return(_tmp892="!",_tag_dyneither(_tmp892,sizeof(char),2));}case Cyc_Absyn_Bitnot:
_LL203: {const char*_tmp893;return(_tmp893="~",_tag_dyneither(_tmp893,sizeof(char),
2));}case Cyc_Absyn_Bitand: _LL204: {const char*_tmp894;return(_tmp894="&",
_tag_dyneither(_tmp894,sizeof(char),2));}case Cyc_Absyn_Bitor: _LL205: {const char*
_tmp895;return(_tmp895="|",_tag_dyneither(_tmp895,sizeof(char),2));}case Cyc_Absyn_Bitxor:
_LL206: {const char*_tmp896;return(_tmp896="^",_tag_dyneither(_tmp896,sizeof(char),
2));}case Cyc_Absyn_Bitlshift: _LL207: {const char*_tmp897;return(_tmp897="<<",
_tag_dyneither(_tmp897,sizeof(char),3));}case Cyc_Absyn_Bitlrshift: _LL208: {const
char*_tmp898;return(_tmp898=">>",_tag_dyneither(_tmp898,sizeof(char),3));}case
Cyc_Absyn_Bitarshift: _LL209: {const char*_tmp899;return(_tmp899=">>>",
_tag_dyneither(_tmp899,sizeof(char),4));}case Cyc_Absyn_Numelts: _LL20A: {const
char*_tmp89A;return(_tmp89A="numelts",_tag_dyneither(_tmp89A,sizeof(char),8));}}}
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p);struct Cyc_PP_Doc*
Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){return Cyc_PP_text(Cyc_Absynpp_prim2str(
p));}int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s){void*_tmp394=s->r;_LL20D: {struct Cyc_Absyn_Decl_s_struct*
_tmp395=(struct Cyc_Absyn_Decl_s_struct*)_tmp394;if(_tmp395->tag != 12)goto _LL20F;}
_LL20E: return 1;_LL20F:;_LL210: return 0;_LL20C:;}struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(
struct Cyc_Absyn_Stmt*st);struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*
st){struct Cyc_PP_Doc*s;{void*_tmp396=st->r;struct Cyc_Absyn_Exp*_tmp399;struct Cyc_Absyn_Stmt*
_tmp39B;struct Cyc_Absyn_Stmt*_tmp39C;struct Cyc_Absyn_Exp*_tmp39E;struct Cyc_Absyn_Exp*
_tmp3A0;struct Cyc_Absyn_Stmt*_tmp3A1;struct Cyc_Absyn_Stmt*_tmp3A2;struct _tuple8
_tmp3A4;struct Cyc_Absyn_Exp*_tmp3A5;struct Cyc_Absyn_Stmt*_tmp3A6;struct
_dyneither_ptr*_tmp3AA;struct Cyc_Absyn_Exp*_tmp3AC;struct _tuple8 _tmp3AD;struct
Cyc_Absyn_Exp*_tmp3AE;struct _tuple8 _tmp3AF;struct Cyc_Absyn_Exp*_tmp3B0;struct Cyc_Absyn_Stmt*
_tmp3B1;struct Cyc_Absyn_Exp*_tmp3B3;struct Cyc_List_List*_tmp3B4;struct Cyc_List_List*
_tmp3B6;struct Cyc_List_List*_tmp3B8;struct Cyc_Absyn_Decl*_tmp3BA;struct Cyc_Absyn_Stmt*
_tmp3BB;struct _dyneither_ptr*_tmp3BD;struct Cyc_Absyn_Stmt*_tmp3BE;struct Cyc_Absyn_Stmt*
_tmp3C0;struct _tuple8 _tmp3C1;struct Cyc_Absyn_Exp*_tmp3C2;struct Cyc_Absyn_Stmt*
_tmp3C4;struct Cyc_List_List*_tmp3C5;struct Cyc_Absyn_Exp*_tmp3C7;_LL212: {struct
Cyc_Absyn_Skip_s_struct*_tmp397=(struct Cyc_Absyn_Skip_s_struct*)_tmp396;if(
_tmp397->tag != 0)goto _LL214;}_LL213:{const char*_tmp89B;s=Cyc_PP_text(((_tmp89B=";",
_tag_dyneither(_tmp89B,sizeof(char),2))));}goto _LL211;_LL214: {struct Cyc_Absyn_Exp_s_struct*
_tmp398=(struct Cyc_Absyn_Exp_s_struct*)_tmp396;if(_tmp398->tag != 1)goto _LL216;
else{_tmp399=_tmp398->f1;}}_LL215:{const char*_tmp89E;struct Cyc_PP_Doc*_tmp89D[2];
s=((_tmp89D[1]=Cyc_PP_text(((_tmp89E=";",_tag_dyneither(_tmp89E,sizeof(char),2)))),((
_tmp89D[0]=Cyc_Absynpp_exp2doc(_tmp399),Cyc_PP_cat(_tag_dyneither(_tmp89D,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL211;_LL216: {struct Cyc_Absyn_Seq_s_struct*
_tmp39A=(struct Cyc_Absyn_Seq_s_struct*)_tmp396;if(_tmp39A->tag != 2)goto _LL218;
else{_tmp39B=_tmp39A->f1;_tmp39C=_tmp39A->f2;}}_LL217: if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp39B)){struct Cyc_PP_Doc*_tmp8A1[5];struct Cyc_PP_Doc*
_tmp8A0[7];s=((_tmp8A0[6]=Cyc_Absynpp_is_declaration(_tmp39C)?(_tmp8A1[4]=Cyc_PP_line_doc(),((
_tmp8A1[3]=Cyc_Absynpp_rb(),((_tmp8A1[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp39C)),((_tmp8A1[1]=Cyc_PP_blank_doc(),((_tmp8A1[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(
_tag_dyneither(_tmp8A1,sizeof(struct Cyc_PP_Doc*),5))))))))))): Cyc_Absynpp_stmt2doc(
_tmp39C),((_tmp8A0[5]=Cyc_PP_line_doc(),((_tmp8A0[4]=Cyc_Absynpp_rb(),((_tmp8A0[
3]=Cyc_PP_line_doc(),((_tmp8A0[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp39B)),((
_tmp8A0[1]=Cyc_PP_blank_doc(),((_tmp8A0[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(
_tag_dyneither(_tmp8A0,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{if(Cyc_Absynpp_is_declaration(
_tmp39C)){struct Cyc_PP_Doc*_tmp8A2[7];s=((_tmp8A2[6]=Cyc_PP_line_doc(),((_tmp8A2[
5]=Cyc_Absynpp_rb(),((_tmp8A2[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp39C)),((
_tmp8A2[3]=Cyc_PP_blank_doc(),((_tmp8A2[2]=Cyc_Absynpp_lb(),((_tmp8A2[1]=Cyc_PP_line_doc(),((
_tmp8A2[0]=Cyc_Absynpp_stmt2doc(_tmp39B),Cyc_PP_cat(_tag_dyneither(_tmp8A2,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{struct Cyc_List_List*_tmp8A6;
struct Cyc_List_List*_tmp8A5;const char*_tmp8A3;s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Stmt*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_stmt2doc,((_tmp8A3="",_tag_dyneither(_tmp8A3,sizeof(char),1))),((
_tmp8A5=_cycalloc(sizeof(*_tmp8A5)),((_tmp8A5->hd=_tmp39B,((_tmp8A5->tl=((
_tmp8A6=_cycalloc(sizeof(*_tmp8A6)),((_tmp8A6->hd=_tmp39C,((_tmp8A6->tl=0,
_tmp8A6)))))),_tmp8A5)))))));}}}else{struct Cyc_List_List*_tmp8AA;struct Cyc_List_List*
_tmp8A9;const char*_tmp8A7;s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct
Cyc_Absyn_Stmt*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_stmt2doc,((_tmp8A7="",_tag_dyneither(_tmp8A7,sizeof(char),1))),((
_tmp8A9=_cycalloc(sizeof(*_tmp8A9)),((_tmp8A9->hd=_tmp39B,((_tmp8A9->tl=((
_tmp8AA=_cycalloc(sizeof(*_tmp8AA)),((_tmp8AA->hd=_tmp39C,((_tmp8AA->tl=0,
_tmp8AA)))))),_tmp8A9)))))));}goto _LL211;_LL218: {struct Cyc_Absyn_Return_s_struct*
_tmp39D=(struct Cyc_Absyn_Return_s_struct*)_tmp396;if(_tmp39D->tag != 3)goto _LL21A;
else{_tmp39E=_tmp39D->f1;}}_LL219: if(_tmp39E == 0){const char*_tmp8AB;s=Cyc_PP_text(((
_tmp8AB="return;",_tag_dyneither(_tmp8AB,sizeof(char),8))));}else{const char*
_tmp8B0;const char*_tmp8AF;struct Cyc_PP_Doc*_tmp8AE[3];s=((_tmp8AE[2]=Cyc_PP_text(((
_tmp8B0=";",_tag_dyneither(_tmp8B0,sizeof(char),2)))),((_tmp8AE[1]=_tmp39E == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp39E),((_tmp8AE[0]=
Cyc_PP_text(((_tmp8AF="return ",_tag_dyneither(_tmp8AF,sizeof(char),8)))),Cyc_PP_cat(
_tag_dyneither(_tmp8AE,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL211;_LL21A: {
struct Cyc_Absyn_IfThenElse_s_struct*_tmp39F=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp396;if(_tmp39F->tag != 4)goto _LL21C;else{_tmp3A0=_tmp39F->f1;_tmp3A1=_tmp39F->f2;
_tmp3A2=_tmp39F->f3;}}_LL21B: {int print_else;{void*_tmp3D8=_tmp3A2->r;_LL237: {
struct Cyc_Absyn_Skip_s_struct*_tmp3D9=(struct Cyc_Absyn_Skip_s_struct*)_tmp3D8;
if(_tmp3D9->tag != 0)goto _LL239;}_LL238: print_else=0;goto _LL236;_LL239:;_LL23A:
print_else=1;goto _LL236;_LL236:;}{struct Cyc_PP_Doc*_tmp8BF[6];const char*_tmp8BE;
struct Cyc_PP_Doc*_tmp8BD[2];struct Cyc_PP_Doc*_tmp8BC[2];const char*_tmp8BB;const
char*_tmp8BA;struct Cyc_PP_Doc*_tmp8B9[8];s=((_tmp8B9[7]=print_else?(_tmp8BF[5]=
Cyc_Absynpp_rb(),((_tmp8BF[4]=Cyc_PP_line_doc(),((_tmp8BF[3]=Cyc_PP_nest(2,((
_tmp8BD[1]=Cyc_Absynpp_stmt2doc(_tmp3A2),((_tmp8BD[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp8BD,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8BF[2]=Cyc_Absynpp_lb(),((
_tmp8BF[1]=Cyc_PP_text(((_tmp8BE="else ",_tag_dyneither(_tmp8BE,sizeof(char),6)))),((
_tmp8BF[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8BF,sizeof(struct Cyc_PP_Doc*),
6))))))))))))): Cyc_PP_nil_doc(),((_tmp8B9[6]=Cyc_Absynpp_rb(),((_tmp8B9[5]=Cyc_PP_line_doc(),((
_tmp8B9[4]=Cyc_PP_nest(2,((_tmp8BC[1]=Cyc_Absynpp_stmt2doc(_tmp3A1),((_tmp8BC[0]=
Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8BC,sizeof(struct Cyc_PP_Doc*),2))))))),((
_tmp8B9[3]=Cyc_Absynpp_lb(),((_tmp8B9[2]=Cyc_PP_text(((_tmp8BB=") ",
_tag_dyneither(_tmp8BB,sizeof(char),3)))),((_tmp8B9[1]=Cyc_Absynpp_exp2doc(
_tmp3A0),((_tmp8B9[0]=Cyc_PP_text(((_tmp8BA="if (",_tag_dyneither(_tmp8BA,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8B9,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL211;}_LL21C: {struct Cyc_Absyn_While_s_struct*_tmp3A3=(struct Cyc_Absyn_While_s_struct*)
_tmp396;if(_tmp3A3->tag != 5)goto _LL21E;else{_tmp3A4=_tmp3A3->f1;_tmp3A5=_tmp3A4.f1;
_tmp3A6=_tmp3A3->f2;}}_LL21D:{struct Cyc_PP_Doc*_tmp8C6[2];const char*_tmp8C5;
const char*_tmp8C4;struct Cyc_PP_Doc*_tmp8C3[7];s=((_tmp8C3[6]=Cyc_Absynpp_rb(),((
_tmp8C3[5]=Cyc_PP_line_doc(),((_tmp8C3[4]=Cyc_PP_nest(2,((_tmp8C6[1]=Cyc_Absynpp_stmt2doc(
_tmp3A6),((_tmp8C6[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8C6,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8C3[3]=Cyc_Absynpp_lb(),((_tmp8C3[2]=
Cyc_PP_text(((_tmp8C5=") ",_tag_dyneither(_tmp8C5,sizeof(char),3)))),((_tmp8C3[1]=
Cyc_Absynpp_exp2doc(_tmp3A5),((_tmp8C3[0]=Cyc_PP_text(((_tmp8C4="while (",
_tag_dyneither(_tmp8C4,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp8C3,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}goto _LL211;_LL21E: {struct Cyc_Absyn_Break_s_struct*
_tmp3A7=(struct Cyc_Absyn_Break_s_struct*)_tmp396;if(_tmp3A7->tag != 6)goto _LL220;}
_LL21F:{const char*_tmp8C7;s=Cyc_PP_text(((_tmp8C7="break;",_tag_dyneither(
_tmp8C7,sizeof(char),7))));}goto _LL211;_LL220: {struct Cyc_Absyn_Continue_s_struct*
_tmp3A8=(struct Cyc_Absyn_Continue_s_struct*)_tmp396;if(_tmp3A8->tag != 7)goto
_LL222;}_LL221:{const char*_tmp8C8;s=Cyc_PP_text(((_tmp8C8="continue;",
_tag_dyneither(_tmp8C8,sizeof(char),10))));}goto _LL211;_LL222: {struct Cyc_Absyn_Goto_s_struct*
_tmp3A9=(struct Cyc_Absyn_Goto_s_struct*)_tmp396;if(_tmp3A9->tag != 8)goto _LL224;
else{_tmp3AA=_tmp3A9->f1;}}_LL223:{const char*_tmp8CC;void*_tmp8CB[1];struct Cyc_String_pa_struct
_tmp8CA;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp8CA.tag=0,((_tmp8CA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp3AA),((_tmp8CB[0]=& _tmp8CA,Cyc_aprintf(((
_tmp8CC="goto %s;",_tag_dyneither(_tmp8CC,sizeof(char),9))),_tag_dyneither(
_tmp8CB,sizeof(void*),1)))))))));}goto _LL211;_LL224: {struct Cyc_Absyn_For_s_struct*
_tmp3AB=(struct Cyc_Absyn_For_s_struct*)_tmp396;if(_tmp3AB->tag != 9)goto _LL226;
else{_tmp3AC=_tmp3AB->f1;_tmp3AD=_tmp3AB->f2;_tmp3AE=_tmp3AD.f1;_tmp3AF=_tmp3AB->f3;
_tmp3B0=_tmp3AF.f1;_tmp3B1=_tmp3AB->f4;}}_LL225:{struct Cyc_PP_Doc*_tmp8D7[2];
const char*_tmp8D6;const char*_tmp8D5;const char*_tmp8D4;const char*_tmp8D3;struct
Cyc_PP_Doc*_tmp8D2[11];s=((_tmp8D2[10]=Cyc_Absynpp_rb(),((_tmp8D2[9]=Cyc_PP_line_doc(),((
_tmp8D2[8]=Cyc_PP_nest(2,((_tmp8D7[1]=Cyc_Absynpp_stmt2doc(_tmp3B1),((_tmp8D7[0]=
Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8D7,sizeof(struct Cyc_PP_Doc*),2))))))),((
_tmp8D2[7]=Cyc_Absynpp_lb(),((_tmp8D2[6]=Cyc_PP_text(((_tmp8D6=") ",
_tag_dyneither(_tmp8D6,sizeof(char),3)))),((_tmp8D2[5]=Cyc_Absynpp_exp2doc(
_tmp3B0),((_tmp8D2[4]=Cyc_PP_text(((_tmp8D5="; ",_tag_dyneither(_tmp8D5,sizeof(
char),3)))),((_tmp8D2[3]=Cyc_Absynpp_exp2doc(_tmp3AE),((_tmp8D2[2]=Cyc_PP_text(((
_tmp8D4="; ",_tag_dyneither(_tmp8D4,sizeof(char),3)))),((_tmp8D2[1]=Cyc_Absynpp_exp2doc(
_tmp3AC),((_tmp8D2[0]=Cyc_PP_text(((_tmp8D3="for(",_tag_dyneither(_tmp8D3,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8D2,sizeof(struct Cyc_PP_Doc*),
11))))))))))))))))))))))));}goto _LL211;_LL226: {struct Cyc_Absyn_Switch_s_struct*
_tmp3B2=(struct Cyc_Absyn_Switch_s_struct*)_tmp396;if(_tmp3B2->tag != 10)goto
_LL228;else{_tmp3B3=_tmp3B2->f1;_tmp3B4=_tmp3B2->f2;}}_LL227:{const char*_tmp8DC;
const char*_tmp8DB;struct Cyc_PP_Doc*_tmp8DA[8];s=((_tmp8DA[7]=Cyc_Absynpp_rb(),((
_tmp8DA[6]=Cyc_PP_line_doc(),((_tmp8DA[5]=Cyc_Absynpp_switchclauses2doc(_tmp3B4),((
_tmp8DA[4]=Cyc_PP_line_doc(),((_tmp8DA[3]=Cyc_Absynpp_lb(),((_tmp8DA[2]=Cyc_PP_text(((
_tmp8DC=") ",_tag_dyneither(_tmp8DC,sizeof(char),3)))),((_tmp8DA[1]=Cyc_Absynpp_exp2doc(
_tmp3B3),((_tmp8DA[0]=Cyc_PP_text(((_tmp8DB="switch (",_tag_dyneither(_tmp8DB,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp8DA,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL211;_LL228: {struct Cyc_Absyn_Fallthru_s_struct*_tmp3B5=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp396;if(_tmp3B5->tag != 11)goto _LL22A;else{_tmp3B6=_tmp3B5->f1;if(_tmp3B6 != 0)
goto _LL22A;}}_LL229:{const char*_tmp8DD;s=Cyc_PP_text(((_tmp8DD="fallthru;",
_tag_dyneither(_tmp8DD,sizeof(char),10))));}goto _LL211;_LL22A: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp3B7=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp396;if(_tmp3B7->tag != 11)goto
_LL22C;else{_tmp3B8=_tmp3B7->f1;}}_LL22B:{const char*_tmp8E2;const char*_tmp8E1;
struct Cyc_PP_Doc*_tmp8E0[3];s=((_tmp8E0[2]=Cyc_PP_text(((_tmp8E2=");",
_tag_dyneither(_tmp8E2,sizeof(char),3)))),((_tmp8E0[1]=Cyc_Absynpp_exps2doc_prec(
20,_tmp3B8),((_tmp8E0[0]=Cyc_PP_text(((_tmp8E1="fallthru(",_tag_dyneither(
_tmp8E1,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp8E0,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LL211;_LL22C: {struct Cyc_Absyn_Decl_s_struct*_tmp3B9=(struct Cyc_Absyn_Decl_s_struct*)
_tmp396;if(_tmp3B9->tag != 12)goto _LL22E;else{_tmp3BA=_tmp3B9->f1;_tmp3BB=_tmp3B9->f2;}}
_LL22D:{struct Cyc_PP_Doc*_tmp8E3[3];s=((_tmp8E3[2]=Cyc_Absynpp_stmt2doc(_tmp3BB),((
_tmp8E3[1]=Cyc_PP_line_doc(),((_tmp8E3[0]=Cyc_Absynpp_decl2doc(_tmp3BA),Cyc_PP_cat(
_tag_dyneither(_tmp8E3,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL211;_LL22E: {
struct Cyc_Absyn_Label_s_struct*_tmp3BC=(struct Cyc_Absyn_Label_s_struct*)_tmp396;
if(_tmp3BC->tag != 13)goto _LL230;else{_tmp3BD=_tmp3BC->f1;_tmp3BE=_tmp3BC->f2;}}
_LL22F: if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp3BE)){const
char*_tmp8E6;struct Cyc_PP_Doc*_tmp8E5[7];s=((_tmp8E5[6]=Cyc_Absynpp_rb(),((
_tmp8E5[5]=Cyc_PP_line_doc(),((_tmp8E5[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3BE)),((_tmp8E5[3]=Cyc_PP_line_doc(),((_tmp8E5[2]=Cyc_Absynpp_lb(),((_tmp8E5[
1]=Cyc_PP_text(((_tmp8E6=": ",_tag_dyneither(_tmp8E6,sizeof(char),3)))),((
_tmp8E5[0]=Cyc_PP_textptr(_tmp3BD),Cyc_PP_cat(_tag_dyneither(_tmp8E5,sizeof(
struct Cyc_PP_Doc*),7))))))))))))))));}else{const char*_tmp8E9;struct Cyc_PP_Doc*
_tmp8E8[3];s=((_tmp8E8[2]=Cyc_Absynpp_stmt2doc(_tmp3BE),((_tmp8E8[1]=Cyc_PP_text(((
_tmp8E9=": ",_tag_dyneither(_tmp8E9,sizeof(char),3)))),((_tmp8E8[0]=Cyc_PP_textptr(
_tmp3BD),Cyc_PP_cat(_tag_dyneither(_tmp8E8,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL211;_LL230: {struct Cyc_Absyn_Do_s_struct*_tmp3BF=(struct Cyc_Absyn_Do_s_struct*)
_tmp396;if(_tmp3BF->tag != 14)goto _LL232;else{_tmp3C0=_tmp3BF->f1;_tmp3C1=_tmp3BF->f2;
_tmp3C2=_tmp3C1.f1;}}_LL231:{const char*_tmp8F0;const char*_tmp8EF;const char*
_tmp8EE;struct Cyc_PP_Doc*_tmp8ED[9];s=((_tmp8ED[8]=Cyc_PP_text(((_tmp8F0=");",
_tag_dyneither(_tmp8F0,sizeof(char),3)))),((_tmp8ED[7]=Cyc_Absynpp_exp2doc(
_tmp3C2),((_tmp8ED[6]=Cyc_PP_text(((_tmp8EF=" while (",_tag_dyneither(_tmp8EF,
sizeof(char),9)))),((_tmp8ED[5]=Cyc_Absynpp_rb(),((_tmp8ED[4]=Cyc_PP_line_doc(),((
_tmp8ED[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3C0)),((_tmp8ED[2]=Cyc_PP_line_doc(),((
_tmp8ED[1]=Cyc_Absynpp_lb(),((_tmp8ED[0]=Cyc_PP_text(((_tmp8EE="do ",
_tag_dyneither(_tmp8EE,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp8ED,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL211;_LL232: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp3C3=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp396;if(_tmp3C3->tag != 15)goto
_LL234;else{_tmp3C4=_tmp3C3->f1;_tmp3C5=_tmp3C3->f2;}}_LL233:{const char*_tmp8F5;
const char*_tmp8F4;struct Cyc_PP_Doc*_tmp8F3[12];s=((_tmp8F3[11]=Cyc_Absynpp_rb(),((
_tmp8F3[10]=Cyc_PP_line_doc(),((_tmp8F3[9]=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(
_tmp3C5)),((_tmp8F3[8]=Cyc_PP_line_doc(),((_tmp8F3[7]=Cyc_Absynpp_lb(),((_tmp8F3[
6]=Cyc_PP_text(((_tmp8F5=" catch ",_tag_dyneither(_tmp8F5,sizeof(char),8)))),((
_tmp8F3[5]=Cyc_Absynpp_rb(),((_tmp8F3[4]=Cyc_PP_line_doc(),((_tmp8F3[3]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp3C4)),((_tmp8F3[2]=Cyc_PP_line_doc(),((_tmp8F3[1]=Cyc_Absynpp_lb(),((
_tmp8F3[0]=Cyc_PP_text(((_tmp8F4="try ",_tag_dyneither(_tmp8F4,sizeof(char),5)))),
Cyc_PP_cat(_tag_dyneither(_tmp8F3,sizeof(struct Cyc_PP_Doc*),12))))))))))))))))))))))))));}
goto _LL211;_LL234: {struct Cyc_Absyn_ResetRegion_s_struct*_tmp3C6=(struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp396;if(_tmp3C6->tag != 16)goto _LL211;else{_tmp3C7=_tmp3C6->f1;}}_LL235:{const
char*_tmp8FA;const char*_tmp8F9;struct Cyc_PP_Doc*_tmp8F8[3];s=((_tmp8F8[2]=Cyc_PP_text(((
_tmp8FA=");",_tag_dyneither(_tmp8FA,sizeof(char),3)))),((_tmp8F8[1]=Cyc_Absynpp_exp2doc(
_tmp3C7),((_tmp8F8[0]=Cyc_PP_text(((_tmp8F9="reset_region(",_tag_dyneither(
_tmp8F9,sizeof(char),14)))),Cyc_PP_cat(_tag_dyneither(_tmp8F8,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LL211;_LL211:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat*p);struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*
p){struct Cyc_PP_Doc*s;{void*_tmp406=p->r;enum Cyc_Absyn_Sign _tmp40A;int _tmp40B;
char _tmp40D;struct _dyneither_ptr _tmp40F;struct Cyc_Absyn_Vardecl*_tmp411;struct
Cyc_Absyn_Pat*_tmp412;struct Cyc_Absyn_Pat _tmp413;void*_tmp414;struct Cyc_Absyn_Vardecl*
_tmp417;struct Cyc_Absyn_Pat*_tmp418;struct Cyc_Absyn_Tvar*_tmp41A;struct Cyc_Absyn_Vardecl*
_tmp41B;struct Cyc_List_List*_tmp41D;int _tmp41E;struct Cyc_Absyn_Pat*_tmp420;
struct Cyc_Absyn_Vardecl*_tmp422;struct Cyc_Absyn_Pat*_tmp423;struct Cyc_Absyn_Pat
_tmp424;void*_tmp425;struct Cyc_Absyn_Vardecl*_tmp428;struct Cyc_Absyn_Pat*_tmp429;
struct _tuple0*_tmp42B;struct _tuple0*_tmp42D;struct Cyc_List_List*_tmp42E;int
_tmp42F;struct Cyc_Absyn_AggrInfo*_tmp431;struct Cyc_Absyn_AggrInfo _tmp432;union
Cyc_Absyn_AggrInfoU _tmp433;struct Cyc_List_List*_tmp434;struct Cyc_List_List*
_tmp435;int _tmp436;struct Cyc_Absyn_AggrInfo*_tmp438;struct Cyc_List_List*_tmp439;
struct Cyc_List_List*_tmp43A;int _tmp43B;struct Cyc_Absyn_Enumfield*_tmp43D;struct
Cyc_Absyn_Enumfield*_tmp43F;struct Cyc_Absyn_Datatypefield*_tmp441;struct Cyc_List_List*
_tmp442;struct Cyc_Absyn_Datatypefield*_tmp444;struct Cyc_List_List*_tmp445;int
_tmp446;struct Cyc_Absyn_Exp*_tmp448;_LL23C: {struct Cyc_Absyn_Wild_p_struct*
_tmp407=(struct Cyc_Absyn_Wild_p_struct*)_tmp406;if(_tmp407->tag != 0)goto _LL23E;}
_LL23D:{const char*_tmp8FB;s=Cyc_PP_text(((_tmp8FB="_",_tag_dyneither(_tmp8FB,
sizeof(char),2))));}goto _LL23B;_LL23E: {struct Cyc_Absyn_Null_p_struct*_tmp408=(
struct Cyc_Absyn_Null_p_struct*)_tmp406;if(_tmp408->tag != 8)goto _LL240;}_LL23F:{
const char*_tmp8FC;s=Cyc_PP_text(((_tmp8FC="NULL",_tag_dyneither(_tmp8FC,sizeof(
char),5))));}goto _LL23B;_LL240: {struct Cyc_Absyn_Int_p_struct*_tmp409=(struct Cyc_Absyn_Int_p_struct*)
_tmp406;if(_tmp409->tag != 9)goto _LL242;else{_tmp40A=_tmp409->f1;_tmp40B=_tmp409->f2;}}
_LL241: if(_tmp40A != (enum Cyc_Absyn_Sign)Cyc_Absyn_Unsigned){const char*_tmp900;
void*_tmp8FF[1];struct Cyc_Int_pa_struct _tmp8FE;s=Cyc_PP_text((struct
_dyneither_ptr)((_tmp8FE.tag=1,((_tmp8FE.f1=(unsigned long)_tmp40B,((_tmp8FF[0]=&
_tmp8FE,Cyc_aprintf(((_tmp900="%d",_tag_dyneither(_tmp900,sizeof(char),3))),
_tag_dyneither(_tmp8FF,sizeof(void*),1)))))))));}else{const char*_tmp904;void*
_tmp903[1];struct Cyc_Int_pa_struct _tmp902;s=Cyc_PP_text((struct _dyneither_ptr)((
_tmp902.tag=1,((_tmp902.f1=(unsigned int)_tmp40B,((_tmp903[0]=& _tmp902,Cyc_aprintf(((
_tmp904="%u",_tag_dyneither(_tmp904,sizeof(char),3))),_tag_dyneither(_tmp903,
sizeof(void*),1)))))))));}goto _LL23B;_LL242: {struct Cyc_Absyn_Char_p_struct*
_tmp40C=(struct Cyc_Absyn_Char_p_struct*)_tmp406;if(_tmp40C->tag != 10)goto _LL244;
else{_tmp40D=_tmp40C->f1;}}_LL243:{const char*_tmp908;void*_tmp907[1];struct Cyc_String_pa_struct
_tmp906;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp906.tag=0,((_tmp906.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp40D)),((
_tmp907[0]=& _tmp906,Cyc_aprintf(((_tmp908="'%s'",_tag_dyneither(_tmp908,sizeof(
char),5))),_tag_dyneither(_tmp907,sizeof(void*),1)))))))));}goto _LL23B;_LL244: {
struct Cyc_Absyn_Float_p_struct*_tmp40E=(struct Cyc_Absyn_Float_p_struct*)_tmp406;
if(_tmp40E->tag != 11)goto _LL246;else{_tmp40F=_tmp40E->f1;}}_LL245: s=Cyc_PP_text(
_tmp40F);goto _LL23B;_LL246: {struct Cyc_Absyn_Var_p_struct*_tmp410=(struct Cyc_Absyn_Var_p_struct*)
_tmp406;if(_tmp410->tag != 1)goto _LL248;else{_tmp411=_tmp410->f1;_tmp412=_tmp410->f2;
_tmp413=*_tmp412;_tmp414=_tmp413.r;{struct Cyc_Absyn_Wild_p_struct*_tmp415=(
struct Cyc_Absyn_Wild_p_struct*)_tmp414;if(_tmp415->tag != 0)goto _LL248;}}}_LL247:
s=Cyc_Absynpp_qvar2doc(_tmp411->name);goto _LL23B;_LL248: {struct Cyc_Absyn_Var_p_struct*
_tmp416=(struct Cyc_Absyn_Var_p_struct*)_tmp406;if(_tmp416->tag != 1)goto _LL24A;
else{_tmp417=_tmp416->f1;_tmp418=_tmp416->f2;}}_LL249:{const char*_tmp90B;struct
Cyc_PP_Doc*_tmp90A[3];s=((_tmp90A[2]=Cyc_Absynpp_pat2doc(_tmp418),((_tmp90A[1]=
Cyc_PP_text(((_tmp90B=" as ",_tag_dyneither(_tmp90B,sizeof(char),5)))),((_tmp90A[
0]=Cyc_Absynpp_qvar2doc(_tmp417->name),Cyc_PP_cat(_tag_dyneither(_tmp90A,sizeof(
struct Cyc_PP_Doc*),3))))))));}goto _LL23B;_LL24A: {struct Cyc_Absyn_TagInt_p_struct*
_tmp419=(struct Cyc_Absyn_TagInt_p_struct*)_tmp406;if(_tmp419->tag != 3)goto _LL24C;
else{_tmp41A=_tmp419->f1;_tmp41B=_tmp419->f2;}}_LL24B:{const char*_tmp910;const
char*_tmp90F;struct Cyc_PP_Doc*_tmp90E[4];s=((_tmp90E[3]=Cyc_PP_text(((_tmp910=">",
_tag_dyneither(_tmp910,sizeof(char),2)))),((_tmp90E[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(
_tmp41A)),((_tmp90E[1]=Cyc_PP_text(((_tmp90F="<",_tag_dyneither(_tmp90F,sizeof(
char),2)))),((_tmp90E[0]=Cyc_Absynpp_qvar2doc(_tmp41B->name),Cyc_PP_cat(
_tag_dyneither(_tmp90E,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL23B;_LL24C: {
struct Cyc_Absyn_Tuple_p_struct*_tmp41C=(struct Cyc_Absyn_Tuple_p_struct*)_tmp406;
if(_tmp41C->tag != 4)goto _LL24E;else{_tmp41D=_tmp41C->f1;_tmp41E=_tmp41C->f2;}}
_LL24D:{const char*_tmp919;const char*_tmp918;const char*_tmp917;const char*_tmp916;
struct Cyc_PP_Doc*_tmp915[4];s=((_tmp915[3]=_tmp41E?Cyc_PP_text(((_tmp918=", ...)",
_tag_dyneither(_tmp918,sizeof(char),7)))): Cyc_PP_text(((_tmp919=")",
_tag_dyneither(_tmp919,sizeof(char),2)))),((_tmp915[2]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,((_tmp917=",",_tag_dyneither(_tmp917,
sizeof(char),2))),_tmp41D),((_tmp915[1]=Cyc_PP_text(((_tmp916="(",_tag_dyneither(
_tmp916,sizeof(char),2)))),((_tmp915[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(
_tag_dyneither(_tmp915,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL23B;_LL24E: {
struct Cyc_Absyn_Pointer_p_struct*_tmp41F=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp406;if(_tmp41F->tag != 5)goto _LL250;else{_tmp420=_tmp41F->f1;}}_LL24F:{const
char*_tmp91C;struct Cyc_PP_Doc*_tmp91B[2];s=((_tmp91B[1]=Cyc_Absynpp_pat2doc(
_tmp420),((_tmp91B[0]=Cyc_PP_text(((_tmp91C="&",_tag_dyneither(_tmp91C,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp91B,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL23B;_LL250: {struct Cyc_Absyn_Reference_p_struct*_tmp421=(struct Cyc_Absyn_Reference_p_struct*)
_tmp406;if(_tmp421->tag != 2)goto _LL252;else{_tmp422=_tmp421->f1;_tmp423=_tmp421->f2;
_tmp424=*_tmp423;_tmp425=_tmp424.r;{struct Cyc_Absyn_Wild_p_struct*_tmp426=(
struct Cyc_Absyn_Wild_p_struct*)_tmp425;if(_tmp426->tag != 0)goto _LL252;}}}_LL251:{
const char*_tmp91F;struct Cyc_PP_Doc*_tmp91E[2];s=((_tmp91E[1]=Cyc_Absynpp_qvar2doc(
_tmp422->name),((_tmp91E[0]=Cyc_PP_text(((_tmp91F="*",_tag_dyneither(_tmp91F,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp91E,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL23B;_LL252: {struct Cyc_Absyn_Reference_p_struct*_tmp427=(struct Cyc_Absyn_Reference_p_struct*)
_tmp406;if(_tmp427->tag != 2)goto _LL254;else{_tmp428=_tmp427->f1;_tmp429=_tmp427->f2;}}
_LL253:{const char*_tmp924;const char*_tmp923;struct Cyc_PP_Doc*_tmp922[4];s=((
_tmp922[3]=Cyc_Absynpp_pat2doc(_tmp429),((_tmp922[2]=Cyc_PP_text(((_tmp924=" as ",
_tag_dyneither(_tmp924,sizeof(char),5)))),((_tmp922[1]=Cyc_Absynpp_qvar2doc(
_tmp428->name),((_tmp922[0]=Cyc_PP_text(((_tmp923="*",_tag_dyneither(_tmp923,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp922,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL23B;_LL254: {struct Cyc_Absyn_UnknownId_p_struct*_tmp42A=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp406;if(_tmp42A->tag != 14)goto _LL256;else{_tmp42B=_tmp42A->f1;}}_LL255: s=Cyc_Absynpp_qvar2doc(
_tmp42B);goto _LL23B;_LL256: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp42C=(
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp406;if(_tmp42C->tag != 15)goto _LL258;
else{_tmp42D=_tmp42C->f1;_tmp42E=_tmp42C->f2;_tmp42F=_tmp42C->f3;}}_LL257: {
const char*_tmp926;const char*_tmp925;struct _dyneither_ptr term=_tmp42F?(_tmp926=", ...)",
_tag_dyneither(_tmp926,sizeof(char),7)):((_tmp925=")",_tag_dyneither(_tmp925,
sizeof(char),2)));{const char*_tmp92B;const char*_tmp92A;struct Cyc_PP_Doc*_tmp929[
2];s=((_tmp929[1]=Cyc_PP_group(((_tmp92B="(",_tag_dyneither(_tmp92B,sizeof(char),
2))),term,((_tmp92A=",",_tag_dyneither(_tmp92A,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_pat2doc,_tmp42E)),((_tmp929[0]=Cyc_Absynpp_qvar2doc(_tmp42D),Cyc_PP_cat(
_tag_dyneither(_tmp929,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL23B;}_LL258: {
struct Cyc_Absyn_Aggr_p_struct*_tmp430=(struct Cyc_Absyn_Aggr_p_struct*)_tmp406;
if(_tmp430->tag != 6)goto _LL25A;else{_tmp431=_tmp430->f1;if(_tmp431 == 0)goto
_LL25A;_tmp432=*_tmp431;_tmp433=_tmp432.aggr_info;_tmp434=_tmp430->f2;_tmp435=
_tmp430->f3;_tmp436=_tmp430->f4;}}_LL259: {const char*_tmp92D;const char*_tmp92C;
struct _dyneither_ptr term=_tmp436?(_tmp92D=", ...}",_tag_dyneither(_tmp92D,
sizeof(char),7)):((_tmp92C="}",_tag_dyneither(_tmp92C,sizeof(char),2)));struct
_tuple0*_tmp46B;struct _tuple9 _tmp46A=Cyc_Absyn_aggr_kinded_name(_tmp433);_tmp46B=
_tmp46A.f2;{const char*_tmp938;const char*_tmp937;const char*_tmp936;const char*
_tmp935;const char*_tmp934;struct Cyc_PP_Doc*_tmp933[4];s=((_tmp933[3]=Cyc_PP_group(((
_tmp938="",_tag_dyneither(_tmp938,sizeof(char),1))),term,((_tmp937=",",
_tag_dyneither(_tmp937,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp435)),((_tmp933[2]=Cyc_PP_egroup(((_tmp936="[",_tag_dyneither(_tmp936,
sizeof(char),2))),((_tmp935="]",_tag_dyneither(_tmp935,sizeof(char),2))),((
_tmp934=",",_tag_dyneither(_tmp934,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp434))),((_tmp933[1]=
Cyc_Absynpp_lb(),((_tmp933[0]=Cyc_Absynpp_qvar2doc(_tmp46B),Cyc_PP_cat(
_tag_dyneither(_tmp933,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL23B;}_LL25A: {
struct Cyc_Absyn_Aggr_p_struct*_tmp437=(struct Cyc_Absyn_Aggr_p_struct*)_tmp406;
if(_tmp437->tag != 6)goto _LL25C;else{_tmp438=_tmp437->f1;if(_tmp438 != 0)goto
_LL25C;_tmp439=_tmp437->f2;_tmp43A=_tmp437->f3;_tmp43B=_tmp437->f4;}}_LL25B: {
const char*_tmp93A;const char*_tmp939;struct _dyneither_ptr term=_tmp43B?(_tmp93A=", ...}",
_tag_dyneither(_tmp93A,sizeof(char),7)):((_tmp939="}",_tag_dyneither(_tmp939,
sizeof(char),2)));{const char*_tmp945;const char*_tmp944;const char*_tmp943;const
char*_tmp942;const char*_tmp941;struct Cyc_PP_Doc*_tmp940[3];s=((_tmp940[2]=Cyc_PP_group(((
_tmp945="",_tag_dyneither(_tmp945,sizeof(char),1))),term,((_tmp944=",",
_tag_dyneither(_tmp944,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp43A)),((_tmp940[1]=Cyc_PP_egroup(((_tmp943="[",_tag_dyneither(_tmp943,
sizeof(char),2))),((_tmp942="]",_tag_dyneither(_tmp942,sizeof(char),2))),((
_tmp941=",",_tag_dyneither(_tmp941,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp439))),((_tmp940[0]=
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp940,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL23B;}_LL25C: {struct Cyc_Absyn_Enum_p_struct*_tmp43C=(struct Cyc_Absyn_Enum_p_struct*)
_tmp406;if(_tmp43C->tag != 12)goto _LL25E;else{_tmp43D=_tmp43C->f2;}}_LL25D:
_tmp43F=_tmp43D;goto _LL25F;_LL25E: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp43E=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp406;if(_tmp43E->tag != 13)goto _LL260;else{
_tmp43F=_tmp43E->f2;}}_LL25F: s=Cyc_Absynpp_qvar2doc(_tmp43F->name);goto _LL23B;
_LL260: {struct Cyc_Absyn_Datatype_p_struct*_tmp440=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp406;if(_tmp440->tag != 7)goto _LL262;else{_tmp441=_tmp440->f2;_tmp442=_tmp440->f3;
if(_tmp442 != 0)goto _LL262;}}_LL261: s=Cyc_Absynpp_qvar2doc(_tmp441->name);goto
_LL23B;_LL262: {struct Cyc_Absyn_Datatype_p_struct*_tmp443=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp406;if(_tmp443->tag != 7)goto _LL264;else{_tmp444=_tmp443->f2;_tmp445=_tmp443->f3;
_tmp446=_tmp443->f4;}}_LL263: {const char*_tmp947;const char*_tmp946;struct
_dyneither_ptr term=_tmp446?(_tmp947=", ...)",_tag_dyneither(_tmp947,sizeof(char),
7)):((_tmp946=")",_tag_dyneither(_tmp946,sizeof(char),2)));{const char*_tmp94C;
const char*_tmp94B;struct Cyc_PP_Doc*_tmp94A[2];s=((_tmp94A[1]=Cyc_PP_egroup(((
_tmp94C="(",_tag_dyneither(_tmp94C,sizeof(char),2))),term,((_tmp94B=",",
_tag_dyneither(_tmp94B,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,
_tmp445)),((_tmp94A[0]=Cyc_Absynpp_qvar2doc(_tmp444->name),Cyc_PP_cat(
_tag_dyneither(_tmp94A,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL23B;}_LL264: {
struct Cyc_Absyn_Exp_p_struct*_tmp447=(struct Cyc_Absyn_Exp_p_struct*)_tmp406;if(
_tmp447->tag != 16)goto _LL23B;else{_tmp448=_tmp447->f1;}}_LL265: s=Cyc_Absynpp_exp2doc(
_tmp448);goto _LL23B;_LL23B:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct
_tuple14*dp);struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple14*dp){const char*
_tmp953;const char*_tmp952;const char*_tmp951;struct Cyc_PP_Doc*_tmp950[2];return(
_tmp950[1]=Cyc_Absynpp_pat2doc((*dp).f2),((_tmp950[0]=Cyc_PP_egroup(((_tmp953="",
_tag_dyneither(_tmp953,sizeof(char),1))),((_tmp952="=",_tag_dyneither(_tmp952,
sizeof(char),2))),((_tmp951="=",_tag_dyneither(_tmp951,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*dp).f1)),Cyc_PP_cat(_tag_dyneither(_tmp950,sizeof(
struct Cyc_PP_Doc*),2)))));}struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct
Cyc_Absyn_Switch_clause*c);struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct
Cyc_Absyn_Switch_clause*c){if(c->where_clause == 0  && (c->pattern)->r == (void*)&
Cyc_Absyn_Wild_p_val){struct Cyc_PP_Doc*_tmp958[2];const char*_tmp957;struct Cyc_PP_Doc*
_tmp956[2];return(_tmp956[1]=Cyc_PP_nest(2,((_tmp958[1]=Cyc_Absynpp_stmt2doc(c->body),((
_tmp958[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp958,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp956[0]=Cyc_PP_text(((_tmp957="default: ",_tag_dyneither(_tmp957,
sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp956,sizeof(struct Cyc_PP_Doc*),
2)))));}else{if(c->where_clause == 0){struct Cyc_PP_Doc*_tmp95F[2];const char*
_tmp95E;const char*_tmp95D;struct Cyc_PP_Doc*_tmp95C[4];return(_tmp95C[3]=Cyc_PP_nest(
2,((_tmp95F[1]=Cyc_Absynpp_stmt2doc(c->body),((_tmp95F[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp95F,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp95C[2]=Cyc_PP_text(((
_tmp95E=": ",_tag_dyneither(_tmp95E,sizeof(char),3)))),((_tmp95C[1]=Cyc_Absynpp_pat2doc(
c->pattern),((_tmp95C[0]=Cyc_PP_text(((_tmp95D="case ",_tag_dyneither(_tmp95D,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp95C,sizeof(struct Cyc_PP_Doc*),4)))))))));}
else{struct Cyc_PP_Doc*_tmp968[2];const char*_tmp967;const char*_tmp966;const char*
_tmp965;struct Cyc_PP_Doc*_tmp964[6];return(_tmp964[5]=Cyc_PP_nest(2,((_tmp968[1]=
Cyc_Absynpp_stmt2doc(c->body),((_tmp968[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp968,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp964[4]=Cyc_PP_text(((
_tmp967=": ",_tag_dyneither(_tmp967,sizeof(char),3)))),((_tmp964[3]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(c->where_clause)),((_tmp964[2]=Cyc_PP_text(((
_tmp966=" && ",_tag_dyneither(_tmp966,sizeof(char),5)))),((_tmp964[1]=Cyc_Absynpp_pat2doc(
c->pattern),((_tmp964[0]=Cyc_PP_text(((_tmp965="case ",_tag_dyneither(_tmp965,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp964,sizeof(struct Cyc_PP_Doc*),6)))))))))))));}}}
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs);struct Cyc_PP_Doc*
Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){const char*_tmp969;return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,((
_tmp969="",_tag_dyneither(_tmp969,sizeof(char),1))),cs);}struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(
struct Cyc_Absyn_Enumfield*f);struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct
Cyc_Absyn_Enumfield*f){if(f->tag == 0)return Cyc_Absynpp_qvar2doc(f->name);else{
const char*_tmp96C;struct Cyc_PP_Doc*_tmp96B[3];return(_tmp96B[2]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->tag)),((_tmp96B[1]=Cyc_PP_text(((_tmp96C=" = ",
_tag_dyneither(_tmp96C,sizeof(char),4)))),((_tmp96B[0]=Cyc_Absynpp_qvar2doc(f->name),
Cyc_PP_cat(_tag_dyneither(_tmp96B,sizeof(struct Cyc_PP_Doc*),3)))))));}}struct Cyc_PP_Doc*
Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs);struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(
struct Cyc_List_List*fs){const char*_tmp96D;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))
Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,((_tmp96D=",",_tag_dyneither(_tmp96D,
sizeof(char),2))),fs);}static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*
v);static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){return
Cyc_Absynpp_qvar2doc(v->name);}static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct
Cyc_List_List*vds);static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*
vds){const char*_tmp96E;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(
Cyc_Absynpp_id2doc,((_tmp96E=",",_tag_dyneither(_tmp96E,sizeof(char),2))),vds);}
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd);struct Cyc_PP_Doc*
Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){struct Cyc_Absyn_Vardecl
_tmp497;enum Cyc_Absyn_Scope _tmp498;struct _tuple0*_tmp499;struct Cyc_Absyn_Tqual
_tmp49A;void*_tmp49B;struct Cyc_Absyn_Exp*_tmp49C;struct Cyc_List_List*_tmp49D;
struct Cyc_Absyn_Vardecl*_tmp496=vd;_tmp497=*_tmp496;_tmp498=_tmp497.sc;_tmp499=
_tmp497.name;_tmp49A=_tmp497.tq;_tmp49B=_tmp497.type;_tmp49C=_tmp497.initializer;
_tmp49D=_tmp497.attributes;{struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(
_tmp499);struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp49D);struct Cyc_PP_Doc*
beforenamedoc;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL266:
beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL267:{void*_tmp49E=Cyc_Tcutil_compress(
_tmp49B);struct Cyc_Absyn_FnInfo _tmp4A0;struct Cyc_List_List*_tmp4A1;_LL26A: {
struct Cyc_Absyn_FnType_struct*_tmp49F=(struct Cyc_Absyn_FnType_struct*)_tmp49E;
if(_tmp49F->tag != 10)goto _LL26C;else{_tmp4A0=_tmp49F->f1;_tmp4A1=_tmp4A0.attributes;}}
_LL26B: beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp4A1);goto _LL269;_LL26C:;_LL26D:
beforenamedoc=Cyc_PP_nil_doc();goto _LL269;_LL269:;}break;}{struct Cyc_PP_Doc*
tmp_doc;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL26E: tmp_doc=Cyc_PP_nil_doc();
break;case Cyc_Cyclone_Vc_c: _LL26F: tmp_doc=attsdoc;break;}{const char*_tmp97B;
struct Cyc_PP_Doc*_tmp97A[2];const char*_tmp979;struct Cyc_Core_Opt*_tmp978;struct
Cyc_PP_Doc*_tmp977[2];struct Cyc_PP_Doc*_tmp976[5];s=((_tmp976[4]=Cyc_PP_text(((
_tmp97B=";",_tag_dyneither(_tmp97B,sizeof(char),2)))),((_tmp976[3]=_tmp49C == 0?
Cyc_PP_nil_doc():((_tmp97A[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp49C),((
_tmp97A[0]=Cyc_PP_text(((_tmp979=" = ",_tag_dyneither(_tmp979,sizeof(char),4)))),
Cyc_PP_cat(_tag_dyneither(_tmp97A,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp976[2]=
Cyc_Absynpp_tqtd2doc(_tmp49A,_tmp49B,((_tmp978=_cycalloc(sizeof(*_tmp978)),((
_tmp978->v=((_tmp977[1]=sn,((_tmp977[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(
_tmp977,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp978))))),((_tmp976[1]=Cyc_Absynpp_scope2doc(
_tmp498),((_tmp976[0]=tmp_doc,Cyc_PP_cat(_tag_dyneither(_tmp976,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}return s;}}}struct _tuple17{struct Cyc_Position_Segment*f1;struct
_tuple0*f2;int f3;};struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x);
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x){struct _tuple0*_tmp4A9;
struct _tuple17 _tmp4A8=*x;_tmp4A9=_tmp4A8.f2;return Cyc_Absynpp_qvar2doc(_tmp4A9);}
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d);struct Cyc_PP_Doc*
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
_tmp4D8;struct Cyc_List_List*_tmp4D9;_LL272: {struct Cyc_Absyn_Fn_d_struct*_tmp4AB=(
struct Cyc_Absyn_Fn_d_struct*)_tmp4AA;if(_tmp4AB->tag != 1)goto _LL274;else{_tmp4AC=
_tmp4AB->f1;}}_LL273: {struct Cyc_Absyn_FnType_struct _tmp981;struct Cyc_Absyn_FnInfo
_tmp980;struct Cyc_Absyn_FnType_struct*_tmp97F;void*t=(void*)((_tmp97F=_cycalloc(
sizeof(*_tmp97F)),((_tmp97F[0]=((_tmp981.tag=10,((_tmp981.f1=((_tmp980.tvars=
_tmp4AC->tvs,((_tmp980.effect=_tmp4AC->effect,((_tmp980.ret_typ=_tmp4AC->ret_type,((
_tmp980.args=((struct Cyc_List_List*(*)(struct _tuple7*(*f)(struct _tuple12*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp4AC->args),((
_tmp980.c_varargs=_tmp4AC->c_varargs,((_tmp980.cyc_varargs=_tmp4AC->cyc_varargs,((
_tmp980.rgn_po=_tmp4AC->rgn_po,((_tmp980.attributes=0,_tmp980)))))))))))))))),
_tmp981)))),_tmp97F))));struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4AC->attributes);
struct Cyc_PP_Doc*inlinedoc;if(_tmp4AC->is_inline)switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LL292:{const char*_tmp982;inlinedoc=Cyc_PP_text(((_tmp982="inline ",
_tag_dyneither(_tmp982,sizeof(char),8))));}break;case Cyc_Cyclone_Vc_c: _LL293:{
const char*_tmp983;inlinedoc=Cyc_PP_text(((_tmp983="__inline ",_tag_dyneither(
_tmp983,sizeof(char),10))));}break;}else{inlinedoc=Cyc_PP_nil_doc();}{struct Cyc_PP_Doc*
scopedoc=Cyc_Absynpp_scope2doc(_tmp4AC->sc);struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL295: beforenamedoc=
attsdoc;break;case Cyc_Cyclone_Vc_c: _LL296: beforenamedoc=Cyc_Absynpp_callconv2doc(
_tmp4AC->attributes);break;}{struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(
_tmp4AC->name);struct Cyc_PP_Doc*_tmp986[2];struct Cyc_Core_Opt*_tmp985;struct Cyc_PP_Doc*
tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,((_tmp985=_cycalloc(
sizeof(*_tmp985)),((_tmp985->v=((_tmp986[1]=namedoc,((_tmp986[0]=beforenamedoc,
Cyc_PP_cat(_tag_dyneither(_tmp986,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp985)))));
struct Cyc_PP_Doc*_tmp989[2];struct Cyc_PP_Doc*_tmp988[5];struct Cyc_PP_Doc*bodydoc=(
_tmp988[4]=Cyc_Absynpp_rb(),((_tmp988[3]=Cyc_PP_line_doc(),((_tmp988[2]=Cyc_PP_nest(
2,((_tmp989[1]=Cyc_Absynpp_stmt2doc(_tmp4AC->body),((_tmp989[0]=Cyc_PP_line_doc(),
Cyc_PP_cat(_tag_dyneither(_tmp989,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp988[1]=
Cyc_Absynpp_lb(),((_tmp988[0]=Cyc_PP_blank_doc(),Cyc_PP_cat(_tag_dyneither(
_tmp988,sizeof(struct Cyc_PP_Doc*),5)))))))))));{struct Cyc_PP_Doc*_tmp98A[4];s=((
_tmp98A[3]=bodydoc,((_tmp98A[2]=tqtddoc,((_tmp98A[1]=scopedoc,((_tmp98A[0]=
inlinedoc,Cyc_PP_cat(_tag_dyneither(_tmp98A,sizeof(struct Cyc_PP_Doc*),4))))))))));}
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL298:{struct Cyc_PP_Doc*
_tmp98B[2];s=((_tmp98B[1]=s,((_tmp98B[0]=attsdoc,Cyc_PP_cat(_tag_dyneither(
_tmp98B,sizeof(struct Cyc_PP_Doc*),2))))));}break;default: _LL299: break;}goto
_LL271;}}}_LL274: {struct Cyc_Absyn_Aggr_d_struct*_tmp4AD=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp4AA;if(_tmp4AD->tag != 6)goto _LL276;else{_tmp4AE=_tmp4AD->f1;}}_LL275: if(
_tmp4AE->impl == 0){const char*_tmp98E;struct Cyc_PP_Doc*_tmp98D[5];s=((_tmp98D[4]=
Cyc_PP_text(((_tmp98E=";",_tag_dyneither(_tmp98E,sizeof(char),2)))),((_tmp98D[3]=
Cyc_Absynpp_ktvars2doc(_tmp4AE->tvs),((_tmp98D[2]=Cyc_Absynpp_qvar2bolddoc(
_tmp4AE->name),((_tmp98D[1]=Cyc_Absynpp_aggr_kind2doc(_tmp4AE->kind),((_tmp98D[0]=
Cyc_Absynpp_scope2doc(_tmp4AE->sc),Cyc_PP_cat(_tag_dyneither(_tmp98D,sizeof(
struct Cyc_PP_Doc*),5))))))))))));}else{const char*_tmp998;struct Cyc_PP_Doc*
_tmp997[2];struct Cyc_PP_Doc*_tmp996[2];const char*_tmp995;struct Cyc_PP_Doc*
_tmp994[13];s=((_tmp994[12]=Cyc_PP_text(((_tmp998=";",_tag_dyneither(_tmp998,
sizeof(char),2)))),((_tmp994[11]=Cyc_Absynpp_atts2doc(_tmp4AE->attributes),((
_tmp994[10]=Cyc_Absynpp_rb(),((_tmp994[9]=Cyc_PP_line_doc(),((_tmp994[8]=Cyc_PP_nest(
2,((_tmp997[1]=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4AE->impl))->fields),((_tmp997[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp997,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp994[7]=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4AE->impl))->rgn_po == 0?Cyc_PP_nil_doc():((
_tmp996[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp4AE->impl))->rgn_po),((_tmp996[0]=Cyc_PP_text(((_tmp995=":",_tag_dyneither(
_tmp995,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp996,sizeof(struct Cyc_PP_Doc*),
2)))))),((_tmp994[6]=Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4AE->impl))->exist_vars),((_tmp994[5]=Cyc_Absynpp_lb(),((_tmp994[
4]=Cyc_PP_blank_doc(),((_tmp994[3]=Cyc_Absynpp_ktvars2doc(_tmp4AE->tvs),((
_tmp994[2]=Cyc_Absynpp_qvar2bolddoc(_tmp4AE->name),((_tmp994[1]=Cyc_Absynpp_aggr_kind2doc(
_tmp4AE->kind),((_tmp994[0]=Cyc_Absynpp_scope2doc(_tmp4AE->sc),Cyc_PP_cat(
_tag_dyneither(_tmp994,sizeof(struct Cyc_PP_Doc*),13))))))))))))))))))))))))))));}
goto _LL271;_LL276: {struct Cyc_Absyn_Var_d_struct*_tmp4AF=(struct Cyc_Absyn_Var_d_struct*)
_tmp4AA;if(_tmp4AF->tag != 0)goto _LL278;else{_tmp4B0=_tmp4AF->f1;}}_LL277: s=Cyc_Absynpp_vardecl2doc(
_tmp4B0);goto _LL271;_LL278: {struct Cyc_Absyn_Region_d_struct*_tmp4B1=(struct Cyc_Absyn_Region_d_struct*)
_tmp4AA;if(_tmp4B1->tag != 4)goto _LL27A;else{_tmp4B2=_tmp4B1->f1;_tmp4B3=_tmp4B1->f2;
_tmp4B4=_tmp4B1->f3;_tmp4B5=_tmp4B1->f4;}}_LL279:{const char*_tmp9AB;struct Cyc_PP_Doc*
_tmp9AA[3];const char*_tmp9A9;const char*_tmp9A8;const char*_tmp9A7;const char*
_tmp9A6;const char*_tmp9A5;const char*_tmp9A4;struct Cyc_PP_Doc*_tmp9A3[8];s=((
_tmp9A3[7]=Cyc_PP_text(((_tmp9AB=";",_tag_dyneither(_tmp9AB,sizeof(char),2)))),((
_tmp9A3[6]=_tmp4B5 != 0?(_tmp9AA[2]=Cyc_PP_text(((_tmp9A8=")",_tag_dyneither(
_tmp9A8,sizeof(char),2)))),((_tmp9AA[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_tmp4B5),((_tmp9AA[0]=Cyc_PP_text(((_tmp9A9=" = open(",_tag_dyneither(_tmp9A9,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp9AA,sizeof(struct Cyc_PP_Doc*),3))))))):
Cyc_PP_nil_doc(),((_tmp9A3[5]=_tmp4B4?Cyc_PP_text(((_tmp9A7=" @resetable",
_tag_dyneither(_tmp9A7,sizeof(char),12)))): Cyc_PP_nil_doc(),((_tmp9A3[4]=Cyc_Absynpp_qvar2doc(
_tmp4B3->name),((_tmp9A3[3]=Cyc_PP_text(((_tmp9A6=">",_tag_dyneither(_tmp9A6,
sizeof(char),2)))),((_tmp9A3[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp4B2)),((
_tmp9A3[1]=Cyc_PP_text(((_tmp9A5="<",_tag_dyneither(_tmp9A5,sizeof(char),2)))),((
_tmp9A3[0]=Cyc_PP_text(((_tmp9A4="region",_tag_dyneither(_tmp9A4,sizeof(char),7)))),
Cyc_PP_cat(_tag_dyneither(_tmp9A3,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL271;_LL27A: {struct Cyc_Absyn_Alias_d_struct*_tmp4B6=(struct Cyc_Absyn_Alias_d_struct*)
_tmp4AA;if(_tmp4B6->tag != 5)goto _LL27C;else{_tmp4B7=_tmp4B6->f1;_tmp4B8=_tmp4B6->f2;
_tmp4B9=_tmp4B6->f3;}}_LL27B:{const char*_tmp9B6;const char*_tmp9B5;const char*
_tmp9B4;const char*_tmp9B3;const char*_tmp9B2;struct Cyc_PP_Doc*_tmp9B1[8];s=((
_tmp9B1[7]=Cyc_PP_text(((_tmp9B6=";",_tag_dyneither(_tmp9B6,sizeof(char),2)))),((
_tmp9B1[6]=Cyc_Absynpp_exp2doc(_tmp4B7),((_tmp9B1[5]=Cyc_PP_text(((_tmp9B5=" = ",
_tag_dyneither(_tmp9B5,sizeof(char),4)))),((_tmp9B1[4]=Cyc_Absynpp_qvar2doc(
_tmp4B9->name),((_tmp9B1[3]=Cyc_PP_text(((_tmp9B4="> ",_tag_dyneither(_tmp9B4,
sizeof(char),3)))),((_tmp9B1[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp4B8)),((
_tmp9B1[1]=Cyc_PP_text(((_tmp9B3=" <",_tag_dyneither(_tmp9B3,sizeof(char),3)))),((
_tmp9B1[0]=Cyc_PP_text(((_tmp9B2="alias",_tag_dyneither(_tmp9B2,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmp9B1,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL271;_LL27C: {struct Cyc_Absyn_Datatype_d_struct*_tmp4BA=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp4AA;if(_tmp4BA->tag != 7)goto _LL27E;else{_tmp4BB=_tmp4BA->f1;_tmp4BC=*_tmp4BB;
_tmp4BD=_tmp4BC.sc;_tmp4BE=_tmp4BC.name;_tmp4BF=_tmp4BC.tvs;_tmp4C0=_tmp4BC.fields;
_tmp4C1=_tmp4BC.is_extensible;}}_LL27D: if(_tmp4C0 == 0){const char*_tmp9BD;const
char*_tmp9BC;const char*_tmp9BB;struct Cyc_PP_Doc*_tmp9BA[6];s=((_tmp9BA[5]=Cyc_PP_text(((
_tmp9BD=";",_tag_dyneither(_tmp9BD,sizeof(char),2)))),((_tmp9BA[4]=Cyc_Absynpp_ktvars2doc(
_tmp4BF),((_tmp9BA[3]=_tmp4C1?Cyc_Absynpp_qvar2bolddoc(_tmp4BE): Cyc_Absynpp_typedef_name2bolddoc(
_tmp4BE),((_tmp9BA[2]=Cyc_PP_text(((_tmp9BC="datatype ",_tag_dyneither(_tmp9BC,
sizeof(char),10)))),((_tmp9BA[1]=_tmp4C1?Cyc_PP_text(((_tmp9BB="@extensible",
_tag_dyneither(_tmp9BB,sizeof(char),12)))): Cyc_PP_blank_doc(),((_tmp9BA[0]=Cyc_Absynpp_scope2doc(
_tmp4BD),Cyc_PP_cat(_tag_dyneither(_tmp9BA,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
else{const char*_tmp9C6;struct Cyc_PP_Doc*_tmp9C5[2];const char*_tmp9C4;const char*
_tmp9C3;struct Cyc_PP_Doc*_tmp9C2[11];s=((_tmp9C2[10]=Cyc_PP_text(((_tmp9C6=";",
_tag_dyneither(_tmp9C6,sizeof(char),2)))),((_tmp9C2[9]=Cyc_Absynpp_rb(),((
_tmp9C2[8]=Cyc_PP_line_doc(),((_tmp9C2[7]=Cyc_PP_nest(2,((_tmp9C5[1]=Cyc_Absynpp_datatypefields2doc((
struct Cyc_List_List*)_tmp4C0->v),((_tmp9C5[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp9C5,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9C2[6]=Cyc_Absynpp_lb(),((
_tmp9C2[5]=Cyc_PP_blank_doc(),((_tmp9C2[4]=Cyc_Absynpp_ktvars2doc(_tmp4BF),((
_tmp9C2[3]=_tmp4C1?Cyc_Absynpp_qvar2bolddoc(_tmp4BE): Cyc_Absynpp_typedef_name2bolddoc(
_tmp4BE),((_tmp9C2[2]=Cyc_PP_text(((_tmp9C4="datatype ",_tag_dyneither(_tmp9C4,
sizeof(char),10)))),((_tmp9C2[1]=_tmp4C1?Cyc_PP_text(((_tmp9C3="@extensible ",
_tag_dyneither(_tmp9C3,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp9C2[0]=Cyc_Absynpp_scope2doc(
_tmp4BD),Cyc_PP_cat(_tag_dyneither(_tmp9C2,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL271;_LL27E: {struct Cyc_Absyn_Let_d_struct*_tmp4C2=(struct Cyc_Absyn_Let_d_struct*)
_tmp4AA;if(_tmp4C2->tag != 2)goto _LL280;else{_tmp4C3=_tmp4C2->f1;_tmp4C4=_tmp4C2->f3;}}
_LL27F:{const char*_tmp9CD;const char*_tmp9CC;const char*_tmp9CB;struct Cyc_PP_Doc*
_tmp9CA[5];s=((_tmp9CA[4]=Cyc_PP_text(((_tmp9CD=";",_tag_dyneither(_tmp9CD,
sizeof(char),2)))),((_tmp9CA[3]=Cyc_Absynpp_exp2doc(_tmp4C4),((_tmp9CA[2]=Cyc_PP_text(((
_tmp9CC=" = ",_tag_dyneither(_tmp9CC,sizeof(char),4)))),((_tmp9CA[1]=Cyc_Absynpp_pat2doc(
_tmp4C3),((_tmp9CA[0]=Cyc_PP_text(((_tmp9CB="let ",_tag_dyneither(_tmp9CB,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp9CA,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL271;_LL280: {struct Cyc_Absyn_Letv_d_struct*_tmp4C5=(struct Cyc_Absyn_Letv_d_struct*)
_tmp4AA;if(_tmp4C5->tag != 3)goto _LL282;else{_tmp4C6=_tmp4C5->f1;}}_LL281:{const
char*_tmp9D2;const char*_tmp9D1;struct Cyc_PP_Doc*_tmp9D0[3];s=((_tmp9D0[2]=Cyc_PP_text(((
_tmp9D2=";",_tag_dyneither(_tmp9D2,sizeof(char),2)))),((_tmp9D0[1]=Cyc_Absynpp_ids2doc(
_tmp4C6),((_tmp9D0[0]=Cyc_PP_text(((_tmp9D1="let ",_tag_dyneither(_tmp9D1,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp9D0,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL271;_LL282: {struct Cyc_Absyn_Enum_d_struct*_tmp4C7=(struct Cyc_Absyn_Enum_d_struct*)
_tmp4AA;if(_tmp4C7->tag != 8)goto _LL284;else{_tmp4C8=_tmp4C7->f1;_tmp4C9=*_tmp4C8;
_tmp4CA=_tmp4C9.sc;_tmp4CB=_tmp4C9.name;_tmp4CC=_tmp4C9.fields;}}_LL283: if(
_tmp4CC == 0){const char*_tmp9D7;const char*_tmp9D6;struct Cyc_PP_Doc*_tmp9D5[4];s=((
_tmp9D5[3]=Cyc_PP_text(((_tmp9D7=";",_tag_dyneither(_tmp9D7,sizeof(char),2)))),((
_tmp9D5[2]=Cyc_Absynpp_typedef_name2bolddoc(_tmp4CB),((_tmp9D5[1]=Cyc_PP_text(((
_tmp9D6="enum ",_tag_dyneither(_tmp9D6,sizeof(char),6)))),((_tmp9D5[0]=Cyc_Absynpp_scope2doc(
_tmp4CA),Cyc_PP_cat(_tag_dyneither(_tmp9D5,sizeof(struct Cyc_PP_Doc*),4))))))))));}
else{const char*_tmp9DE;struct Cyc_PP_Doc*_tmp9DD[2];const char*_tmp9DC;struct Cyc_PP_Doc*
_tmp9DB[9];s=((_tmp9DB[8]=Cyc_PP_text(((_tmp9DE=";",_tag_dyneither(_tmp9DE,
sizeof(char),2)))),((_tmp9DB[7]=Cyc_Absynpp_rb(),((_tmp9DB[6]=Cyc_PP_line_doc(),((
_tmp9DB[5]=Cyc_PP_nest(2,((_tmp9DD[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)
_tmp4CC->v),((_tmp9DD[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9DD,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9DB[4]=Cyc_Absynpp_lb(),((_tmp9DB[3]=
Cyc_PP_blank_doc(),((_tmp9DB[2]=Cyc_Absynpp_qvar2bolddoc(_tmp4CB),((_tmp9DB[1]=
Cyc_PP_text(((_tmp9DC="enum ",_tag_dyneither(_tmp9DC,sizeof(char),6)))),((
_tmp9DB[0]=Cyc_Absynpp_scope2doc(_tmp4CA),Cyc_PP_cat(_tag_dyneither(_tmp9DB,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL271;_LL284: {struct Cyc_Absyn_Typedef_d_struct*
_tmp4CD=(struct Cyc_Absyn_Typedef_d_struct*)_tmp4AA;if(_tmp4CD->tag != 9)goto
_LL286;else{_tmp4CE=_tmp4CD->f1;}}_LL285: {void*t;if(_tmp4CE->defn != 0)t=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4CE->defn))->v;else{t=Cyc_Absyn_new_evar(
_tmp4CE->kind,0);}{const char*_tmp9E8;struct Cyc_Core_Opt*_tmp9E7;struct Cyc_PP_Doc*
_tmp9E6[2];const char*_tmp9E5;struct Cyc_PP_Doc*_tmp9E4[4];s=((_tmp9E4[3]=Cyc_PP_text(((
_tmp9E8=";",_tag_dyneither(_tmp9E8,sizeof(char),2)))),((_tmp9E4[2]=Cyc_Absynpp_atts2doc(
_tmp4CE->atts),((_tmp9E4[1]=Cyc_Absynpp_tqtd2doc(_tmp4CE->tq,t,((_tmp9E7=
_cycalloc(sizeof(*_tmp9E7)),((_tmp9E7->v=((_tmp9E6[1]=Cyc_Absynpp_tvars2doc(
_tmp4CE->tvs),((_tmp9E6[0]=Cyc_Absynpp_typedef_name2bolddoc(_tmp4CE->name),Cyc_PP_cat(
_tag_dyneither(_tmp9E6,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9E7))))),((_tmp9E4[
0]=Cyc_PP_text(((_tmp9E5="typedef ",_tag_dyneither(_tmp9E5,sizeof(char),9)))),
Cyc_PP_cat(_tag_dyneither(_tmp9E4,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto
_LL271;}_LL286: {struct Cyc_Absyn_Namespace_d_struct*_tmp4CF=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp4AA;if(_tmp4CF->tag != 10)goto _LL288;else{_tmp4D0=_tmp4CF->f1;_tmp4D1=_tmp4CF->f2;}}
_LL287: if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp4D0);{
const char*_tmp9ED;const char*_tmp9EC;struct Cyc_PP_Doc*_tmp9EB[8];s=((_tmp9EB[7]=
Cyc_Absynpp_rb(),((_tmp9EB[6]=Cyc_PP_line_doc(),((_tmp9EB[5]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmp9ED="",_tag_dyneither(_tmp9ED,
sizeof(char),1))),_tmp4D1),((_tmp9EB[4]=Cyc_PP_line_doc(),((_tmp9EB[3]=Cyc_Absynpp_lb(),((
_tmp9EB[2]=Cyc_PP_blank_doc(),((_tmp9EB[1]=Cyc_PP_textptr(_tmp4D0),((_tmp9EB[0]=
Cyc_PP_text(((_tmp9EC="namespace ",_tag_dyneither(_tmp9EC,sizeof(char),11)))),
Cyc_PP_cat(_tag_dyneither(_tmp9EB,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();goto _LL271;
_LL288: {struct Cyc_Absyn_Using_d_struct*_tmp4D2=(struct Cyc_Absyn_Using_d_struct*)
_tmp4AA;if(_tmp4D2->tag != 11)goto _LL28A;else{_tmp4D3=_tmp4D2->f1;_tmp4D4=_tmp4D2->f2;}}
_LL289: if(Cyc_Absynpp_print_using_stmts){const char*_tmp9F2;const char*_tmp9F1;
struct Cyc_PP_Doc*_tmp9F0[8];s=((_tmp9F0[7]=Cyc_Absynpp_rb(),((_tmp9F0[6]=Cyc_PP_line_doc(),((
_tmp9F0[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmp9F2="",_tag_dyneither(_tmp9F2,sizeof(char),1))),_tmp4D4),((_tmp9F0[4]=Cyc_PP_line_doc(),((
_tmp9F0[3]=Cyc_Absynpp_lb(),((_tmp9F0[2]=Cyc_PP_blank_doc(),((_tmp9F0[1]=Cyc_Absynpp_qvar2doc(
_tmp4D3),((_tmp9F0[0]=Cyc_PP_text(((_tmp9F1="using ",_tag_dyneither(_tmp9F1,
sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp9F0,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
else{const char*_tmp9FD;const char*_tmp9FC;const char*_tmp9FB;const char*_tmp9FA;
const char*_tmp9F9;struct Cyc_PP_Doc*_tmp9F8[11];s=((_tmp9F8[10]=Cyc_PP_text(((
_tmp9FD=" */",_tag_dyneither(_tmp9FD,sizeof(char),4)))),((_tmp9F8[9]=Cyc_Absynpp_rb(),((
_tmp9F8[8]=Cyc_PP_text(((_tmp9FC="/* ",_tag_dyneither(_tmp9FC,sizeof(char),4)))),((
_tmp9F8[7]=Cyc_PP_line_doc(),((_tmp9F8[6]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmp9FB="",_tag_dyneither(_tmp9FB,sizeof(char),1))),
_tmp4D4),((_tmp9F8[5]=Cyc_PP_line_doc(),((_tmp9F8[4]=Cyc_PP_text(((_tmp9FA=" */",
_tag_dyneither(_tmp9FA,sizeof(char),4)))),((_tmp9F8[3]=Cyc_Absynpp_lb(),((
_tmp9F8[2]=Cyc_PP_blank_doc(),((_tmp9F8[1]=Cyc_Absynpp_qvar2doc(_tmp4D3),((
_tmp9F8[0]=Cyc_PP_text(((_tmp9F9="/* using ",_tag_dyneither(_tmp9F9,sizeof(char),
10)))),Cyc_PP_cat(_tag_dyneither(_tmp9F8,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL271;_LL28A: {struct Cyc_Absyn_ExternC_d_struct*_tmp4D5=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp4AA;if(_tmp4D5->tag != 12)goto _LL28C;else{_tmp4D6=_tmp4D5->f1;}}_LL28B: if(Cyc_Absynpp_print_externC_stmts){
const char*_tmpA02;const char*_tmpA01;struct Cyc_PP_Doc*_tmpA00[6];s=((_tmpA00[5]=
Cyc_Absynpp_rb(),((_tmpA00[4]=Cyc_PP_line_doc(),((_tmpA00[3]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA02="",_tag_dyneither(_tmpA02,
sizeof(char),1))),_tmp4D6),((_tmpA00[2]=Cyc_PP_line_doc(),((_tmpA00[1]=Cyc_Absynpp_lb(),((
_tmpA00[0]=Cyc_PP_text(((_tmpA01="extern \"C\" ",_tag_dyneither(_tmpA01,sizeof(
char),12)))),Cyc_PP_cat(_tag_dyneither(_tmpA00,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
else{const char*_tmpA0D;const char*_tmpA0C;const char*_tmpA0B;const char*_tmpA0A;
const char*_tmpA09;struct Cyc_PP_Doc*_tmpA08[9];s=((_tmpA08[8]=Cyc_PP_text(((
_tmpA0D=" */",_tag_dyneither(_tmpA0D,sizeof(char),4)))),((_tmpA08[7]=Cyc_Absynpp_rb(),((
_tmpA08[6]=Cyc_PP_text(((_tmpA0C="/* ",_tag_dyneither(_tmpA0C,sizeof(char),4)))),((
_tmpA08[5]=Cyc_PP_line_doc(),((_tmpA08[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmpA0B="",_tag_dyneither(_tmpA0B,sizeof(char),1))),
_tmp4D6),((_tmpA08[3]=Cyc_PP_line_doc(),((_tmpA08[2]=Cyc_PP_text(((_tmpA0A=" */",
_tag_dyneither(_tmpA0A,sizeof(char),4)))),((_tmpA08[1]=Cyc_Absynpp_lb(),((
_tmpA08[0]=Cyc_PP_text(((_tmpA09="/* extern \"C\" ",_tag_dyneither(_tmpA09,
sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmpA08,sizeof(struct Cyc_PP_Doc*),
9))))))))))))))))))));}goto _LL271;_LL28C: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp4D7=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp4AA;if(_tmp4D7->tag != 13)
goto _LL28E;else{_tmp4D8=_tmp4D7->f1;_tmp4D9=_tmp4D7->f2;}}_LL28D: if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;if(_tmp4D9 != 0){const char*_tmpA12;const char*_tmpA11;
struct Cyc_PP_Doc*_tmpA10[7];exs_doc=((_tmpA10[6]=Cyc_Absynpp_rb(),((_tmpA10[5]=
Cyc_PP_line_doc(),((_tmpA10[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_export2doc,((_tmpA12=",",_tag_dyneither(_tmpA12,sizeof(char),2))),
_tmp4D9),((_tmpA10[3]=Cyc_PP_line_doc(),((_tmpA10[2]=Cyc_Absynpp_lb(),((_tmpA10[
1]=Cyc_PP_text(((_tmpA11=" export ",_tag_dyneither(_tmpA11,sizeof(char),9)))),((
_tmpA10[0]=Cyc_Absynpp_rb(),Cyc_PP_cat(_tag_dyneither(_tmpA10,sizeof(struct Cyc_PP_Doc*),
7))))))))))))))));}else{exs_doc=Cyc_Absynpp_rb();}{const char*_tmpA17;const char*
_tmpA16;struct Cyc_PP_Doc*_tmpA15[6];s=((_tmpA15[5]=exs_doc,((_tmpA15[4]=Cyc_PP_line_doc(),((
_tmpA15[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA17="",_tag_dyneither(_tmpA17,sizeof(char),1))),_tmp4D8),((_tmpA15[2]=Cyc_PP_line_doc(),((
_tmpA15[1]=Cyc_Absynpp_lb(),((_tmpA15[0]=Cyc_PP_text(((_tmpA16="extern \"C include\" ",
_tag_dyneither(_tmpA16,sizeof(char),20)))),Cyc_PP_cat(_tag_dyneither(_tmpA15,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));}}else{const char*_tmpA22;const char*
_tmpA21;const char*_tmpA20;const char*_tmpA1F;const char*_tmpA1E;struct Cyc_PP_Doc*
_tmpA1D[9];s=((_tmpA1D[8]=Cyc_PP_text(((_tmpA22=" */",_tag_dyneither(_tmpA22,
sizeof(char),4)))),((_tmpA1D[7]=Cyc_Absynpp_rb(),((_tmpA1D[6]=Cyc_PP_text(((
_tmpA21="/* ",_tag_dyneither(_tmpA21,sizeof(char),4)))),((_tmpA1D[5]=Cyc_PP_line_doc(),((
_tmpA1D[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA20="",_tag_dyneither(_tmpA20,sizeof(char),1))),_tmp4D8),((_tmpA1D[3]=Cyc_PP_line_doc(),((
_tmpA1D[2]=Cyc_PP_text(((_tmpA1F=" */",_tag_dyneither(_tmpA1F,sizeof(char),4)))),((
_tmpA1D[1]=Cyc_Absynpp_lb(),((_tmpA1D[0]=Cyc_PP_text(((_tmpA1E="/* extern \"C include\" ",
_tag_dyneither(_tmpA1E,sizeof(char),23)))),Cyc_PP_cat(_tag_dyneither(_tmpA1D,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL271;_LL28E: {struct Cyc_Absyn_Porton_d_struct*
_tmp4DA=(struct Cyc_Absyn_Porton_d_struct*)_tmp4AA;if(_tmp4DA->tag != 14)goto
_LL290;}_LL28F:{const char*_tmpA25;struct Cyc_PP_Doc*_tmpA24[2];s=((_tmpA24[1]=Cyc_Absynpp_lb(),((
_tmpA24[0]=Cyc_PP_text(((_tmpA25="__cyclone_port_on__;",_tag_dyneither(_tmpA25,
sizeof(char),21)))),Cyc_PP_cat(_tag_dyneither(_tmpA24,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LL271;_LL290: {struct Cyc_Absyn_Portoff_d_struct*_tmp4DB=(struct Cyc_Absyn_Portoff_d_struct*)
_tmp4AA;if(_tmp4DB->tag != 15)goto _LL271;}_LL291:{const char*_tmpA28;struct Cyc_PP_Doc*
_tmpA27[2];s=((_tmpA27[1]=Cyc_Absynpp_lb(),((_tmpA27[0]=Cyc_PP_text(((_tmpA28="__cyclone_port_off__;",
_tag_dyneither(_tmpA28,sizeof(char),22)))),Cyc_PP_cat(_tag_dyneither(_tmpA27,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL271;_LL271:;}return s;}struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc);struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(
enum Cyc_Absyn_Scope sc){if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();
switch(sc){case Cyc_Absyn_Static: _LL29B: {const char*_tmpA29;return Cyc_PP_text(((
_tmpA29="static ",_tag_dyneither(_tmpA29,sizeof(char),8))));}case Cyc_Absyn_Public:
_LL29C: return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL29D: {const char*_tmpA2A;
return Cyc_PP_text(((_tmpA2A="extern ",_tag_dyneither(_tmpA2A,sizeof(char),8))));}
case Cyc_Absyn_ExternC: _LL29E: {const char*_tmpA2B;return Cyc_PP_text(((_tmpA2B="extern \"C\" ",
_tag_dyneither(_tmpA2B,sizeof(char),12))));}case Cyc_Absyn_Abstract: _LL29F: {
const char*_tmpA2C;return Cyc_PP_text(((_tmpA2C="abstract ",_tag_dyneither(_tmpA2C,
sizeof(char),10))));}case Cyc_Absyn_Register: _LL2A0: {const char*_tmpA2D;return Cyc_PP_text(((
_tmpA2D="register ",_tag_dyneither(_tmpA2D,sizeof(char),10))));}}}int Cyc_Absynpp_exists_temp_tvar_in_effect(
void*t);int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){void*_tmp543=t;struct
Cyc_Absyn_Tvar*_tmp545;struct Cyc_List_List*_tmp547;_LL2A3: {struct Cyc_Absyn_VarType_struct*
_tmp544=(struct Cyc_Absyn_VarType_struct*)_tmp543;if(_tmp544->tag != 2)goto _LL2A5;
else{_tmp545=_tmp544->f1;}}_LL2A4: return Cyc_Tcutil_is_temp_tvar(_tmp545);_LL2A5: {
struct Cyc_Absyn_JoinEff_struct*_tmp546=(struct Cyc_Absyn_JoinEff_struct*)_tmp543;
if(_tmp546->tag != 24)goto _LL2A7;else{_tmp547=_tmp546->f1;}}_LL2A6: return((int(*)(
int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,
_tmp547);_LL2A7:;_LL2A8: return 0;_LL2A2:;}int Cyc_Absynpp_is_anon_aggrtype(void*t);
int Cyc_Absynpp_is_anon_aggrtype(void*t){void*_tmp548=t;void**_tmp54C;void*
_tmp54D;_LL2AA: {struct Cyc_Absyn_AnonAggrType_struct*_tmp549=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp548;if(_tmp549->tag != 13)goto _LL2AC;}_LL2AB: return 1;_LL2AC: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp54A=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp548;if(_tmp54A->tag != 15)goto
_LL2AE;}_LL2AD: return 1;_LL2AE: {struct Cyc_Absyn_TypedefType_struct*_tmp54B=(
struct Cyc_Absyn_TypedefType_struct*)_tmp548;if(_tmp54B->tag != 18)goto _LL2B0;
else{_tmp54C=_tmp54B->f4;if(_tmp54C == 0)goto _LL2B0;_tmp54D=*_tmp54C;}}_LL2AF:
return Cyc_Absynpp_is_anon_aggrtype(_tmp54D);_LL2B0:;_LL2B1: return 0;_LL2A9:;}
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,
void*atts,struct Cyc_List_List*tms);static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(
struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){if(tms != 0  && tms->tl != 
0){struct _tuple11 _tmpA2E;struct _tuple11 _tmp54F=(_tmpA2E.f1=(void*)tms->hd,((
_tmpA2E.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,_tmpA2E)));
void*_tmp550;void*_tmp552;_LL2B3: _tmp550=_tmp54F.f1;{struct Cyc_Absyn_Pointer_mod_struct*
_tmp551=(struct Cyc_Absyn_Pointer_mod_struct*)_tmp550;if(_tmp551->tag != 2)goto
_LL2B5;}_tmp552=_tmp54F.f2;{struct Cyc_Absyn_Function_mod_struct*_tmp553=(struct
Cyc_Absyn_Function_mod_struct*)_tmp552;if(_tmp553->tag != 3)goto _LL2B5;}_LL2B4: {
struct Cyc_List_List*_tmpA31;struct Cyc_List_List*_tmpA30;return(_tmpA30=
_region_malloc(r,sizeof(*_tmpA30)),((_tmpA30->hd=(void*)tms->hd,((_tmpA30->tl=((
_tmpA31=_region_malloc(r,sizeof(*_tmpA31)),((_tmpA31->hd=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd,((_tmpA31->tl=Cyc_Absynpp_bubble_attributes(r,atts,((
struct Cyc_List_List*)_check_null(tms->tl))->tl),_tmpA31)))))),_tmpA30)))));}
_LL2B5:;_LL2B6: {struct Cyc_List_List*_tmpA32;return(_tmpA32=_region_malloc(r,
sizeof(*_tmpA32)),((_tmpA32->hd=atts,((_tmpA32->tl=tms,_tmpA32)))));}_LL2B2:;}
else{struct Cyc_List_List*_tmpA33;return(_tmpA33=_region_malloc(r,sizeof(*_tmpA33)),((
_tmpA33->hd=atts,((_tmpA33->tl=tms,_tmpA33)))));}}struct _tuple13 Cyc_Absynpp_to_tms(
struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t);struct _tuple13 Cyc_Absynpp_to_tms(
struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){void*_tmp558=t;struct Cyc_Absyn_ArrayInfo
_tmp55A;void*_tmp55B;struct Cyc_Absyn_Tqual _tmp55C;struct Cyc_Absyn_Exp*_tmp55D;
union Cyc_Absyn_Constraint*_tmp55E;struct Cyc_Position_Segment*_tmp55F;struct Cyc_Absyn_PtrInfo
_tmp561;void*_tmp562;struct Cyc_Absyn_Tqual _tmp563;struct Cyc_Absyn_PtrAtts _tmp564;
struct Cyc_Absyn_FnInfo _tmp566;struct Cyc_List_List*_tmp567;struct Cyc_Core_Opt*
_tmp568;void*_tmp569;struct Cyc_List_List*_tmp56A;int _tmp56B;struct Cyc_Absyn_VarargInfo*
_tmp56C;struct Cyc_List_List*_tmp56D;struct Cyc_List_List*_tmp56E;struct Cyc_Core_Opt*
_tmp570;struct Cyc_Core_Opt*_tmp571;int _tmp572;struct _tuple0*_tmp574;struct Cyc_List_List*
_tmp575;void**_tmp576;_LL2B8: {struct Cyc_Absyn_ArrayType_struct*_tmp559=(struct
Cyc_Absyn_ArrayType_struct*)_tmp558;if(_tmp559->tag != 9)goto _LL2BA;else{_tmp55A=
_tmp559->f1;_tmp55B=_tmp55A.elt_type;_tmp55C=_tmp55A.tq;_tmp55D=_tmp55A.num_elts;
_tmp55E=_tmp55A.zero_term;_tmp55F=_tmp55A.zt_loc;}}_LL2B9: {struct Cyc_Absyn_Tqual
_tmp578;void*_tmp579;struct Cyc_List_List*_tmp57A;struct _tuple13 _tmp577=Cyc_Absynpp_to_tms(
r,_tmp55C,_tmp55B);_tmp578=_tmp577.f1;_tmp579=_tmp577.f2;_tmp57A=_tmp577.f3;{
void*tm;if(_tmp55D == 0){struct Cyc_Absyn_Carray_mod_struct _tmpA36;struct Cyc_Absyn_Carray_mod_struct*
_tmpA35;tm=(void*)((_tmpA35=_region_malloc(r,sizeof(*_tmpA35)),((_tmpA35[0]=((
_tmpA36.tag=0,((_tmpA36.f1=_tmp55E,((_tmpA36.f2=_tmp55F,_tmpA36)))))),_tmpA35))));}
else{struct Cyc_Absyn_ConstArray_mod_struct _tmpA39;struct Cyc_Absyn_ConstArray_mod_struct*
_tmpA38;tm=(void*)((_tmpA38=_region_malloc(r,sizeof(*_tmpA38)),((_tmpA38[0]=((
_tmpA39.tag=1,((_tmpA39.f1=(struct Cyc_Absyn_Exp*)_tmp55D,((_tmpA39.f2=_tmp55E,((
_tmpA39.f3=_tmp55F,_tmpA39)))))))),_tmpA38))));}{struct Cyc_List_List*_tmpA3C;
struct _tuple13 _tmpA3B;return(_tmpA3B.f1=_tmp578,((_tmpA3B.f2=_tmp579,((_tmpA3B.f3=((
_tmpA3C=_region_malloc(r,sizeof(*_tmpA3C)),((_tmpA3C->hd=tm,((_tmpA3C->tl=
_tmp57A,_tmpA3C)))))),_tmpA3B)))));}}}_LL2BA: {struct Cyc_Absyn_PointerType_struct*
_tmp560=(struct Cyc_Absyn_PointerType_struct*)_tmp558;if(_tmp560->tag != 5)goto
_LL2BC;else{_tmp561=_tmp560->f1;_tmp562=_tmp561.elt_typ;_tmp563=_tmp561.elt_tq;
_tmp564=_tmp561.ptr_atts;}}_LL2BB: {struct Cyc_Absyn_Tqual _tmp582;void*_tmp583;
struct Cyc_List_List*_tmp584;struct _tuple13 _tmp581=Cyc_Absynpp_to_tms(r,_tmp563,
_tmp562);_tmp582=_tmp581.f1;_tmp583=_tmp581.f2;_tmp584=_tmp581.f3;{struct Cyc_Absyn_Pointer_mod_struct*
_tmpA42;struct Cyc_Absyn_Pointer_mod_struct _tmpA41;struct Cyc_List_List*_tmpA40;
_tmp584=((_tmpA40=_region_malloc(r,sizeof(*_tmpA40)),((_tmpA40->hd=(void*)((
_tmpA42=_region_malloc(r,sizeof(*_tmpA42)),((_tmpA42[0]=((_tmpA41.tag=2,((
_tmpA41.f1=_tmp564,((_tmpA41.f2=tq,_tmpA41)))))),_tmpA42)))),((_tmpA40->tl=
_tmp584,_tmpA40))))));}{struct _tuple13 _tmpA43;return(_tmpA43.f1=_tmp582,((
_tmpA43.f2=_tmp583,((_tmpA43.f3=_tmp584,_tmpA43)))));}}_LL2BC: {struct Cyc_Absyn_FnType_struct*
_tmp565=(struct Cyc_Absyn_FnType_struct*)_tmp558;if(_tmp565->tag != 10)goto _LL2BE;
else{_tmp566=_tmp565->f1;_tmp567=_tmp566.tvars;_tmp568=_tmp566.effect;_tmp569=
_tmp566.ret_typ;_tmp56A=_tmp566.args;_tmp56B=_tmp566.c_varargs;_tmp56C=_tmp566.cyc_varargs;
_tmp56D=_tmp566.rgn_po;_tmp56E=_tmp566.attributes;}}_LL2BD: if(!Cyc_Absynpp_print_all_tvars){
if(_tmp568 == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect((void*)_tmp568->v)){
_tmp568=0;_tmp567=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,
_tmp567);}{struct Cyc_Absyn_Tqual _tmp58A;void*_tmp58B;struct Cyc_List_List*_tmp58C;
struct _tuple13 _tmp589=Cyc_Absynpp_to_tms(r,Cyc_Absyn_empty_tqual(0),_tmp569);
_tmp58A=_tmp589.f1;_tmp58B=_tmp589.f2;_tmp58C=_tmp589.f3;{struct Cyc_List_List*
tms=_tmp58C;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2C4: if(
_tmp56E != 0){struct Cyc_Absyn_Attributes_mod_struct _tmpA46;struct Cyc_Absyn_Attributes_mod_struct*
_tmpA45;tms=Cyc_Absynpp_bubble_attributes(r,(void*)((_tmpA45=_region_malloc(r,
sizeof(*_tmpA45)),((_tmpA45[0]=((_tmpA46.tag=5,((_tmpA46.f1=0,((_tmpA46.f2=
_tmp56E,_tmpA46)))))),_tmpA45)))),tms);}{struct Cyc_Absyn_Function_mod_struct*
_tmpA55;struct Cyc_Absyn_WithTypes_struct*_tmpA54;struct Cyc_Absyn_WithTypes_struct
_tmpA53;struct Cyc_Absyn_Function_mod_struct _tmpA52;struct Cyc_List_List*_tmpA51;
tms=((_tmpA51=_region_malloc(r,sizeof(*_tmpA51)),((_tmpA51->hd=(void*)((_tmpA55=
_region_malloc(r,sizeof(*_tmpA55)),((_tmpA55[0]=((_tmpA52.tag=3,((_tmpA52.f1=(
void*)((void*)((_tmpA54=_region_malloc(r,sizeof(*_tmpA54)),((_tmpA54[0]=((
_tmpA53.tag=1,((_tmpA53.f1=_tmp56A,((_tmpA53.f2=_tmp56B,((_tmpA53.f3=_tmp56C,((
_tmpA53.f4=_tmp568,((_tmpA53.f5=_tmp56D,_tmpA53)))))))))))),_tmpA54))))),_tmpA52)))),
_tmpA55)))),((_tmpA51->tl=tms,_tmpA51))))));}break;case Cyc_Cyclone_Vc_c: _LL2C5:{
struct Cyc_Absyn_Function_mod_struct*_tmpA64;struct Cyc_Absyn_WithTypes_struct*
_tmpA63;struct Cyc_Absyn_WithTypes_struct _tmpA62;struct Cyc_Absyn_Function_mod_struct
_tmpA61;struct Cyc_List_List*_tmpA60;tms=((_tmpA60=_region_malloc(r,sizeof(*
_tmpA60)),((_tmpA60->hd=(void*)((_tmpA64=_region_malloc(r,sizeof(*_tmpA64)),((
_tmpA64[0]=((_tmpA61.tag=3,((_tmpA61.f1=(void*)((void*)((_tmpA63=_region_malloc(
r,sizeof(*_tmpA63)),((_tmpA63[0]=((_tmpA62.tag=1,((_tmpA62.f1=_tmp56A,((_tmpA62.f2=
_tmp56B,((_tmpA62.f3=_tmp56C,((_tmpA62.f4=_tmp568,((_tmpA62.f5=_tmp56D,_tmpA62)))))))))))),
_tmpA63))))),_tmpA61)))),_tmpA64)))),((_tmpA60->tl=tms,_tmpA60))))));}for(0;
_tmp56E != 0;_tmp56E=_tmp56E->tl){void*_tmp599=(void*)_tmp56E->hd;_LL2C8: {struct
Cyc_Absyn_Stdcall_att_struct*_tmp59A=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmp599;if(_tmp59A->tag != 1)goto _LL2CA;}_LL2C9: goto _LL2CB;_LL2CA: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp59B=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp599;if(_tmp59B->tag != 2)goto
_LL2CC;}_LL2CB: goto _LL2CD;_LL2CC: {struct Cyc_Absyn_Fastcall_att_struct*_tmp59C=(
struct Cyc_Absyn_Fastcall_att_struct*)_tmp599;if(_tmp59C->tag != 3)goto _LL2CE;}
_LL2CD:{struct Cyc_Absyn_Attributes_mod_struct*_tmpA6E;struct Cyc_List_List*
_tmpA6D;struct Cyc_Absyn_Attributes_mod_struct _tmpA6C;struct Cyc_List_List*_tmpA6B;
tms=((_tmpA6B=_region_malloc(r,sizeof(*_tmpA6B)),((_tmpA6B->hd=(void*)((_tmpA6E=
_region_malloc(r,sizeof(*_tmpA6E)),((_tmpA6E[0]=((_tmpA6C.tag=5,((_tmpA6C.f1=0,((
_tmpA6C.f2=((_tmpA6D=_cycalloc(sizeof(*_tmpA6D)),((_tmpA6D->hd=(void*)_tmp56E->hd,((
_tmpA6D->tl=0,_tmpA6D)))))),_tmpA6C)))))),_tmpA6E)))),((_tmpA6B->tl=tms,_tmpA6B))))));}
goto AfterAtts;_LL2CE:;_LL2CF: goto _LL2C7;_LL2C7:;}break;}AfterAtts: if(_tmp567 != 0){
struct Cyc_Absyn_TypeParams_mod_struct*_tmpA74;struct Cyc_Absyn_TypeParams_mod_struct
_tmpA73;struct Cyc_List_List*_tmpA72;tms=((_tmpA72=_region_malloc(r,sizeof(*
_tmpA72)),((_tmpA72->hd=(void*)((_tmpA74=_region_malloc(r,sizeof(*_tmpA74)),((
_tmpA74[0]=((_tmpA73.tag=4,((_tmpA73.f1=_tmp567,((_tmpA73.f2=0,((_tmpA73.f3=1,
_tmpA73)))))))),_tmpA74)))),((_tmpA72->tl=tms,_tmpA72))))));}{struct _tuple13
_tmpA75;return(_tmpA75.f1=_tmp58A,((_tmpA75.f2=_tmp58B,((_tmpA75.f3=tms,_tmpA75)))));}}}
_LL2BE: {struct Cyc_Absyn_Evar_struct*_tmp56F=(struct Cyc_Absyn_Evar_struct*)
_tmp558;if(_tmp56F->tag != 1)goto _LL2C0;else{_tmp570=_tmp56F->f1;_tmp571=_tmp56F->f2;
_tmp572=_tmp56F->f3;}}_LL2BF: if(_tmp571 == 0){struct _tuple13 _tmpA76;return(
_tmpA76.f1=tq,((_tmpA76.f2=t,((_tmpA76.f3=0,_tmpA76)))));}else{return Cyc_Absynpp_to_tms(
r,tq,(void*)_tmp571->v);}_LL2C0: {struct Cyc_Absyn_TypedefType_struct*_tmp573=(
struct Cyc_Absyn_TypedefType_struct*)_tmp558;if(_tmp573->tag != 18)goto _LL2C2;
else{_tmp574=_tmp573->f1;_tmp575=_tmp573->f2;_tmp576=_tmp573->f4;}}_LL2C1: if((
_tmp576 == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype(*
_tmp576)){struct _tuple13 _tmpA77;return(_tmpA77.f1=tq,((_tmpA77.f2=t,((_tmpA77.f3=
0,_tmpA77)))));}else{return Cyc_Absynpp_to_tms(r,tq,*_tmp576);}_LL2C2:;_LL2C3: {
struct _tuple13 _tmpA78;return(_tmpA78.f1=tq,((_tmpA78.f2=t,((_tmpA78.f3=0,_tmpA78)))));}
_LL2B7:;}static int Cyc_Absynpp_is_char_ptr(void*t);static int Cyc_Absynpp_is_char_ptr(
void*t){void*_tmp5A8=t;struct Cyc_Core_Opt*_tmp5AA;struct Cyc_Core_Opt _tmp5AB;void*
_tmp5AC;struct Cyc_Absyn_PtrInfo _tmp5AE;void*_tmp5AF;_LL2D1: {struct Cyc_Absyn_Evar_struct*
_tmp5A9=(struct Cyc_Absyn_Evar_struct*)_tmp5A8;if(_tmp5A9->tag != 1)goto _LL2D3;
else{_tmp5AA=_tmp5A9->f2;if(_tmp5AA == 0)goto _LL2D3;_tmp5AB=*_tmp5AA;_tmp5AC=(
void*)_tmp5AB.v;}}_LL2D2: return Cyc_Absynpp_is_char_ptr(_tmp5AC);_LL2D3: {struct
Cyc_Absyn_PointerType_struct*_tmp5AD=(struct Cyc_Absyn_PointerType_struct*)
_tmp5A8;if(_tmp5AD->tag != 5)goto _LL2D5;else{_tmp5AE=_tmp5AD->f1;_tmp5AF=_tmp5AE.elt_typ;}}
_LL2D4: L: {void*_tmp5B0=_tmp5AF;struct Cyc_Core_Opt*_tmp5B2;struct Cyc_Core_Opt
_tmp5B3;void*_tmp5B4;void**_tmp5B6;void*_tmp5B7;enum Cyc_Absyn_Size_of _tmp5B9;
_LL2D8: {struct Cyc_Absyn_Evar_struct*_tmp5B1=(struct Cyc_Absyn_Evar_struct*)
_tmp5B0;if(_tmp5B1->tag != 1)goto _LL2DA;else{_tmp5B2=_tmp5B1->f2;if(_tmp5B2 == 0)
goto _LL2DA;_tmp5B3=*_tmp5B2;_tmp5B4=(void*)_tmp5B3.v;}}_LL2D9: _tmp5AF=_tmp5B4;
goto L;_LL2DA: {struct Cyc_Absyn_TypedefType_struct*_tmp5B5=(struct Cyc_Absyn_TypedefType_struct*)
_tmp5B0;if(_tmp5B5->tag != 18)goto _LL2DC;else{_tmp5B6=_tmp5B5->f4;if(_tmp5B6 == 0)
goto _LL2DC;_tmp5B7=*_tmp5B6;}}_LL2DB: _tmp5AF=_tmp5B7;goto L;_LL2DC: {struct Cyc_Absyn_IntType_struct*
_tmp5B8=(struct Cyc_Absyn_IntType_struct*)_tmp5B0;if(_tmp5B8->tag != 6)goto _LL2DE;
else{_tmp5B9=_tmp5B8->f2;if(_tmp5B9 != Cyc_Absyn_Char_sz)goto _LL2DE;}}_LL2DD:
return 1;_LL2DE:;_LL2DF: return 0;_LL2D7:;}_LL2D5:;_LL2D6: return 0;_LL2D0:;}struct
Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*
dopt);struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,
struct Cyc_Core_Opt*dopt){struct _RegionHandle _tmp5BA=_new_region("temp");struct
_RegionHandle*temp=& _tmp5BA;_push_region(temp);{struct Cyc_Absyn_Tqual _tmp5BC;
void*_tmp5BD;struct Cyc_List_List*_tmp5BE;struct _tuple13 _tmp5BB=Cyc_Absynpp_to_tms(
temp,tq,typ);_tmp5BC=_tmp5BB.f1;_tmp5BD=_tmp5BB.f2;_tmp5BE=_tmp5BB.f3;_tmp5BE=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5BE);if(
_tmp5BE == 0  && dopt == 0){struct Cyc_PP_Doc*_tmpA79[2];struct Cyc_PP_Doc*_tmp5C0=(
_tmpA79[1]=Cyc_Absynpp_ntyp2doc(_tmp5BD),((_tmpA79[0]=Cyc_Absynpp_tqual2doc(
_tmp5BC),Cyc_PP_cat(_tag_dyneither(_tmpA79,sizeof(struct Cyc_PP_Doc*),2)))));
_npop_handler(0);return _tmp5C0;}else{const char*_tmpA7C;struct Cyc_PP_Doc*_tmpA7B[
4];struct Cyc_PP_Doc*_tmp5C3=(_tmpA7B[3]=Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(
typ),dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp5BE),((_tmpA7B[2]=
Cyc_PP_text(((_tmpA7C=" ",_tag_dyneither(_tmpA7C,sizeof(char),2)))),((_tmpA7B[1]=
Cyc_Absynpp_ntyp2doc(_tmp5BD),((_tmpA7B[0]=Cyc_Absynpp_tqual2doc(_tmp5BC),Cyc_PP_cat(
_tag_dyneither(_tmpA7B,sizeof(struct Cyc_PP_Doc*),4)))))))));_npop_handler(0);
return _tmp5C3;}};_pop_region(temp);}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(
struct Cyc_Absyn_Aggrfield*f);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct
Cyc_Absyn_Aggrfield*f){switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c:
_LL2E0: if(f->width != 0){const char*_tmpA83;const char*_tmpA82;struct Cyc_Core_Opt*
_tmpA81;struct Cyc_PP_Doc*_tmpA80[5];return(_tmpA80[4]=Cyc_PP_text(((_tmpA83=";",
_tag_dyneither(_tmpA83,sizeof(char),2)))),((_tmpA80[3]=Cyc_Absynpp_atts2doc(f->attributes),((
_tmpA80[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((
_tmpA80[1]=Cyc_PP_text(((_tmpA82=":",_tag_dyneither(_tmpA82,sizeof(char),2)))),((
_tmpA80[0]=Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpA81=_cycalloc(sizeof(*
_tmpA81)),((_tmpA81->v=Cyc_PP_textptr(f->name),_tmpA81))))),Cyc_PP_cat(
_tag_dyneither(_tmpA80,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{const char*
_tmpA88;struct Cyc_Core_Opt*_tmpA87;struct Cyc_PP_Doc*_tmpA86[3];return(_tmpA86[2]=
Cyc_PP_text(((_tmpA88=";",_tag_dyneither(_tmpA88,sizeof(char),2)))),((_tmpA86[1]=
Cyc_Absynpp_atts2doc(f->attributes),((_tmpA86[0]=Cyc_Absynpp_tqtd2doc(f->tq,f->type,((
_tmpA87=_cycalloc(sizeof(*_tmpA87)),((_tmpA87->v=Cyc_PP_textptr(f->name),_tmpA87))))),
Cyc_PP_cat(_tag_dyneither(_tmpA86,sizeof(struct Cyc_PP_Doc*),3)))))));}case Cyc_Cyclone_Vc_c:
_LL2E1: if(f->width != 0){const char*_tmpA8F;const char*_tmpA8E;struct Cyc_Core_Opt*
_tmpA8D;struct Cyc_PP_Doc*_tmpA8C[5];return(_tmpA8C[4]=Cyc_PP_text(((_tmpA8F=";",
_tag_dyneither(_tmpA8F,sizeof(char),2)))),((_tmpA8C[3]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpA8C[2]=Cyc_PP_text(((_tmpA8E=":",
_tag_dyneither(_tmpA8E,sizeof(char),2)))),((_tmpA8C[1]=Cyc_Absynpp_tqtd2doc(f->tq,
f->type,((_tmpA8D=_cycalloc(sizeof(*_tmpA8D)),((_tmpA8D->v=Cyc_PP_textptr(f->name),
_tmpA8D))))),((_tmpA8C[0]=Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(
_tag_dyneither(_tmpA8C,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{const char*
_tmpA94;struct Cyc_Core_Opt*_tmpA93;struct Cyc_PP_Doc*_tmpA92[3];return(_tmpA92[2]=
Cyc_PP_text(((_tmpA94=";",_tag_dyneither(_tmpA94,sizeof(char),2)))),((_tmpA92[1]=
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpA93=_cycalloc(sizeof(*_tmpA93)),((
_tmpA93->v=Cyc_PP_textptr(f->name),_tmpA93))))),((_tmpA92[0]=Cyc_Absynpp_atts2doc(
f->attributes),Cyc_PP_cat(_tag_dyneither(_tmpA92,sizeof(struct Cyc_PP_Doc*),3)))))));}}}
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields);struct
Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){const char*
_tmpA95;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,((
_tmpA95="",_tag_dyneither(_tmpA95,sizeof(char),1))),fields);}struct Cyc_PP_Doc*
Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f);struct Cyc_PP_Doc*
Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){struct Cyc_PP_Doc*
_tmpA96[3];return(_tmpA96[2]=f->typs == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs),((
_tmpA96[1]=Cyc_Absynpp_typedef_name2doc(f->name),((_tmpA96[0]=Cyc_Absynpp_scope2doc(
f->sc),Cyc_PP_cat(_tag_dyneither(_tmpA96,sizeof(struct Cyc_PP_Doc*),3)))))));}
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields);
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
const char*_tmpA97;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,((
_tmpA97=",",_tag_dyneither(_tmpA97,sizeof(char),2))),fields);}void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){for(0;tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(
Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);{const char*_tmpA9A;
void*_tmpA99;(_tmpA99=0,Cyc_fprintf(f,((_tmpA9A="\n",_tag_dyneither(_tmpA9A,
sizeof(char),2))),_tag_dyneither(_tmpA99,sizeof(void*),0)));}}}struct
_dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);struct
_dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){const char*
_tmpA9B;return Cyc_PP_string_of_doc(Cyc_PP_seql(((_tmpA9B="",_tag_dyneither(
_tmpA9B,sizeof(char),1))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl)),
72);}struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(
Cyc_Absynpp_exp2doc(e),72);}struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct
Cyc_Absyn_Stmt*s);struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*
s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s),72);}struct _dyneither_ptr
Cyc_Absynpp_typ2string(void*t);struct _dyneither_ptr Cyc_Absynpp_typ2string(void*t){
return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}struct _dyneither_ptr Cyc_Absynpp_typ2cstring(
void*t);struct _dyneither_ptr Cyc_Absynpp_typ2cstring(void*t){int old_qvar_to_Cids=
Cyc_Absynpp_qvar_to_Cids;int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;Cyc_Absynpp_qvar_to_Cids=
1;Cyc_Absynpp_add_cyc_prefix=0;{struct _dyneither_ptr s=Cyc_Absynpp_typ2string(t);
Cyc_Absynpp_qvar_to_Cids=old_qvar_to_Cids;Cyc_Absynpp_add_cyc_prefix=
old_add_cyc_prefix;return s;}}struct _dyneither_ptr Cyc_Absynpp_prim2string(enum 
Cyc_Absyn_Primop p);struct _dyneither_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop
p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}struct _dyneither_ptr
Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p);struct _dyneither_ptr Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc);struct
_dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(
Cyc_Absynpp_scope2doc(sc),72);}
