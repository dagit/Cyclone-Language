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
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
struct _dyneither_ptr Cstring_to_string(char*);struct Cyc___cycFILE;extern struct Cyc___cycFILE*
Cyc_stdout;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fclose(struct Cyc___cycFILE*);int Cyc_fflush(struct
Cyc___cycFILE*);struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);int Cyc_fprintf(
struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);int Cyc_fputc(int,
struct Cyc___cycFILE*);int Cyc_fputs(const char*,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
int remove(const char*);int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,
struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(
struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int
lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;
int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct
_dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr
lex_check;};struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
struct _dyneither_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);char Cyc_Lexing_lexeme_char(
struct Cyc_Lexing_lexbuf*,int);int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;
struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*
tag;};void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{
char*tag;};int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*
x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_empty(int(*
cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);
struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);
struct Cyc_Set_Set*Cyc_Set_diff(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);struct
Cyc_Set_Set*Cyc_Set_delete(struct Cyc_Set_Set*s,void*elt);int Cyc_Set_cardinality(
struct Cyc_Set_Set*s);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char
Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Iter_Iter Cyc_Set_make_iter(
struct _RegionHandle*rgn,struct Cyc_Set_Set*s);unsigned long Cyc_strlen(struct
_dyneither_ptr s);int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*
s2);struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
struct _dyneither_ptr Cyc_strdup(struct _dyneither_ptr src);struct _dyneither_ptr Cyc_substring(
struct _dyneither_ptr,int ofs,unsigned long n);struct Cyc_Hashtable_Table;struct Cyc_Hashtable_Table*
Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(
struct Cyc_Hashtable_Table*t,void*key,void*val);void*Cyc_Hashtable_lookup(struct
Cyc_Hashtable_Table*t,void*key);int Cyc_Hashtable_hash_stringptr(struct
_dyneither_ptr*p);struct _dyneither_ptr Cyc_Filename_concat(struct _dyneither_ptr,
struct _dyneither_ptr);struct _dyneither_ptr Cyc_Filename_chop_extension(struct
_dyneither_ptr);struct _dyneither_ptr Cyc_Filename_dirname(struct _dyneither_ptr);
struct _dyneither_ptr Cyc_Filename_basename(struct _dyneither_ptr);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};void Cyc_Position_reset_position(
struct _dyneither_ptr);struct Cyc_Position_Segment;struct Cyc_Position_Lex_struct{
int tag;};struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{
int tag;};struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];
struct Cyc_Position_Nocontext_struct{char*tag;};extern char Cyc_Arg_Bad[4];struct
Cyc_Arg_Bad_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[6];
struct Cyc_Arg_Error_struct{char*tag;};struct Cyc_Arg_Unit_spec_struct{int tag;void(*
f1)();};struct Cyc_Arg_Flag_spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};
struct Cyc_Arg_FlagString_spec_struct{int tag;void(*f1)(struct _dyneither_ptr,
struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_struct{
int tag;int*f1;};struct Cyc_Arg_String_spec_struct{int tag;void(*f1)(struct
_dyneither_ptr);};struct Cyc_Arg_Int_spec_struct{int tag;void(*f1)(int);};struct
Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};void Cyc_Arg_usage(
struct Cyc_List_List*,struct _dyneither_ptr);void Cyc_Arg_parse(struct Cyc_List_List*
specs,void(*anonfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct
_dyneither_ptr args);struct Cyc_Buffer_t;struct Cyc_Buffer_t*Cyc_Buffer_create(
unsigned int n);struct _dyneither_ptr Cyc_Buffer_contents(struct Cyc_Buffer_t*);void
Cyc_Buffer_add_char(struct Cyc_Buffer_t*,char);void Cyc_Buffer_add_string(struct
Cyc_Buffer_t*,struct _dyneither_ptr);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
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
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;
};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
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
char*tag;};struct _tuple9{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};struct
_tuple9 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f);enum Cyc_Storage_class{Cyc_Typedef_sc  = 0,Cyc_Extern_sc
 = 1,Cyc_ExternC_sc  = 2,Cyc_Static_sc  = 3,Cyc_Auto_sc  = 4,Cyc_Register_sc  = 5,
Cyc_Abstract_sc  = 6};struct Cyc_Declaration_spec;struct Cyc_Declarator;struct Cyc_Abstractdeclarator;
struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct
_union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;
struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*
val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple10{
struct Cyc_Position_Segment*f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*
f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple10*val;};struct
_union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY3{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY4{int
tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY7{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY8{
int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Stmt*
val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY11{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple11{struct Cyc_List_List*
f1;int f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple11*val;};struct
_union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _tuple12{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY14{int tag;struct _tuple12*val;
};struct _union_YYSTYPE_YY15{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY16{int tag;struct _tuple11*val;};struct _union_YYSTYPE_YY17{int tag;
struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_Declaration_spec*val;};struct
_tuple13{struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct
_union_YYSTYPE_YY20{int tag;struct _tuple13*val;};struct _union_YYSTYPE_YY21{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Storage_class
val;};struct _union_YYSTYPE_YY23{int tag;void*val;};struct _union_YYSTYPE_YY24{int
tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual
val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{
int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_Declarator*
val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator*val;};struct
_union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope
val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};
struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple14{struct
Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct
_union_YYSTYPE_YY35{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY36{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple7*val;};
struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple15{struct
Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;
struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple15*val;};
struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{
int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;enum Cyc_Absyn_Kind val;};
struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct
_union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct
_union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{
int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{
int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};
struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct
_union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct
_union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok
Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_YY1
YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct
_union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;
struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9
YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct
_union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14
YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct
_union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19
YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct
_union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24
YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct
_union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29
YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct
_union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34
YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct
_union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39
YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct
_union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44
YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct
_union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49
YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct
_union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54
YY54;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int
timestamp;int first_line;int first_column;int last_line;int last_column;};struct Cyc_Dict_T;
struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*
t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*tag;};
extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};struct
_tuple16{void*f1;void*f2;};struct _tuple16*Cyc_Dict_rchoose(struct _RegionHandle*r,
struct Cyc_Dict_Dict d);struct _tuple16*Cyc_Dict_rchoose(struct _RegionHandle*,
struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tc_init(struct _RegionHandle*);void Cyc_Tc_tc(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);typedef
struct{int quot;int rem;}Cyc_div_t;typedef struct{long quot;long rem;}Cyc_ldiv_t;void*
abort();void exit(int);int system(const char*);struct Cyc_timespec{long tv_sec;int
tv_nsec;};struct Cyc_stat{int st_dev;unsigned int st_ino;unsigned short st_mode;
unsigned short st_nlink;unsigned int st_uid;unsigned int st_gid;int st_rdev;struct
Cyc_timespec st_atimespec;struct Cyc_timespec st_mtimespec;struct Cyc_timespec
st_ctimespec;long long st_size;long long st_blocks;unsigned int st_blksize;
unsigned int st_flags;unsigned int st_gen;int st_lspare;long long st_qspare[2];};int
mkdir(const char*pathname,unsigned short mode);struct Cyc_flock{long long l_start;
long long l_len;int l_pid;short l_type;short l_whence;};struct Cyc_Flock_struct{int
tag;struct Cyc_flock*f1;};struct Cyc_Long_struct{int tag;long f1;};int Cyc_open(const
char*,int,struct _dyneither_ptr);struct Cyc_option{struct _dyneither_ptr name;int
has_arg;int*flag;int val;};int chdir(const char*);int close(int);struct
_dyneither_ptr Cyc_getcwd(struct _dyneither_ptr buf,unsigned long size);int isspace(
int);int toupper(int);void Cyc_Lex_lex_init(int use_cyclone_keywords);extern char*
Ccomp;static int Cyc_do_setjmp=0;struct Cyc___cycFILE*Cyc_log_file=0;struct Cyc___cycFILE*
Cyc_cstubs_file=0;struct Cyc___cycFILE*Cyc_cycstubs_file=0;int Cyc_log(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);int Cyc_log(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap){if(Cyc_log_file == 0){{const char*_tmp3A5;void*_tmp3A4;(
_tmp3A4=0,Cyc_fprintf(Cyc_stderr,((_tmp3A5="Internal error: log file is NULL\n",
_tag_dyneither(_tmp3A5,sizeof(char),34))),_tag_dyneither(_tmp3A4,sizeof(void*),0)));}
exit(1);}{int _tmp2=Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_log_file),
fmt,ap);Cyc_fflush((struct Cyc___cycFILE*)((struct Cyc___cycFILE*)_check_null(Cyc_log_file)));
return _tmp2;};}static struct _dyneither_ptr*Cyc_current_source=0;static struct Cyc_List_List*
Cyc_current_args=0;static struct Cyc_Set_Set**Cyc_current_targets=0;static void Cyc_add_target(
struct _dyneither_ptr*sptr);static void Cyc_add_target(struct _dyneither_ptr*sptr){
struct Cyc_Set_Set**_tmp3A6;Cyc_current_targets=((_tmp3A6=_cycalloc(sizeof(*
_tmp3A6)),((_tmp3A6[0]=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),
sptr),_tmp3A6))));}struct _tuple17{struct _dyneither_ptr*f1;struct Cyc_Set_Set*f2;}
;struct _tuple17*Cyc_line(struct Cyc_Lexing_lexbuf*);int Cyc_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_args(struct Cyc_Lexing_lexbuf*);int Cyc_token(struct Cyc_Lexing_lexbuf*);int
Cyc_string(struct Cyc_Lexing_lexbuf*);struct Cyc___cycFILE*Cyc_slurp_out=0;int Cyc_slurp(
struct Cyc_Lexing_lexbuf*);int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*);int Cyc_asm(
struct Cyc_Lexing_lexbuf*);int Cyc_asm_string(struct Cyc_Lexing_lexbuf*);int Cyc_asm_comment(
struct Cyc_Lexing_lexbuf*);struct _tuple18{struct _dyneither_ptr f1;struct
_dyneither_ptr*f2;};struct _tuple18*Cyc_suck_line(struct Cyc_Lexing_lexbuf*);int
Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*);int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*);
struct _dyneither_ptr Cyc_current_line=(struct _dyneither_ptr){(void*)0,(void*)0,(
void*)(0 + 0)};struct _tuple19{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};
struct _tuple20{struct _dyneither_ptr f1;struct Cyc_List_List*f2;struct Cyc_List_List*
f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};struct
_tuple20*Cyc_spec(struct Cyc_Lexing_lexbuf*);int Cyc_commands(struct Cyc_Lexing_lexbuf*);
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*);int Cyc_block(struct Cyc_Lexing_lexbuf*);
int Cyc_block_string(struct Cyc_Lexing_lexbuf*);int Cyc_block_comment(struct Cyc_Lexing_lexbuf*);
struct _dyneither_ptr Cyc_current_headerfile=(struct _dyneither_ptr){(void*)0,(void*)
0,(void*)(0 + 0)};struct Cyc_List_List*Cyc_snarfed_symbols=0;struct Cyc_List_List*
Cyc_current_symbols=0;struct Cyc_List_List*Cyc_current_cstubs=0;struct Cyc_List_List*
Cyc_current_cycstubs=0;struct Cyc_List_List*Cyc_current_hstubs=0;struct Cyc_List_List*
Cyc_current_omit_symbols=0;struct Cyc_List_List*Cyc_current_cpp=0;struct Cyc_Buffer_t*
Cyc_specbuf=0;int Cyc_braces_to_match=0;int Cyc_parens_to_match=0;int Cyc_numdef=0;
const int Cyc_lex_base[406]=(const int[406]){0,0,75,192,305,310,311,166,312,91,27,
384,28,523,637,715,283,325,92,- 3,0,- 1,- 2,- 8,- 3,1,- 2,323,- 4,2,166,- 5,605,797,312,
- 6,- 7,- 4,16,- 3,29,11,835,- 3,910,13,- 14,223,12,- 2,216,20,26,28,33,23,48,70,54,64,
74,100,91,107,94,370,386,111,103,95,122,122,375,414,111,111,153,377,1024,1139,
525,176,210,228,214,216,217,242,498,1253,1368,- 9,654,- 10,224,245,508,1482,1597,
684,- 8,720,- 11,432,510,515,1674,1751,1828,1909,434,465,550,1984,249,249,249,248,
244,254,0,13,4,2065,5,628,2073,2138,660,49,467,6,2099,7,705,2161,2199,820,- 21,
1051,1056,261,314,242,251,258,250,271,281,274,275,278,288,295,282,- 19,292,296,
300,313,321,326,309,325,319,315,326,365,400,417,418,404,400,400,427,431,- 16,422,
421,415,436,433,449,427,449,453,441,445,436,436,- 18,444,438,442,453,464,447,449,
482,489,490,1,4,6,491,492,505,504,516,516,524,554,2,21,556,557,654,19,20,21,592,
555,554,585,592,594,23,650,651,- 13,600,603,658,659,660,622,623,678,679,686,636,
637,693,697,698,645,648,703,704,705,- 12,654,655,1030,- 20,1165,654,664,661,671,
670,665,668,698,700,698,712,1144,730,731,730,743,1258,1170,747,758,748,749,747,
760,1370,752,753,765,778,1375,- 7,- 8,8,1263,2231,9,996,2255,2293,1341,1279,- 49,
1150,- 2,817,- 4,818,997,1028,819,993,1023,1448,820,2320,2363,824,866,868,907,2433,
870,991,- 36,- 42,- 37,2508,- 28,909,- 40,- 25,911,- 27,- 45,- 39,- 48,2583,2612,1467,889,
979,1563,2622,2652,1582,2281,2685,2716,2754,1001,1090,2824,2862,1082,1092,1084,
1127,1148,1204,- 6,- 34,927,2794,- 47,- 30,- 32,- 46,- 29,- 31,- 33,962,2902,963,964,2128,
965,1002,1003,1014,1018,1020,1024,1032,1043,2975,3059,- 23,- 17,- 15,- 22,2239,1077,
- 24,- 41,- 38,- 35,- 26,1282,3141,4,3224,1076,15,1051,1055,1056,1058,1054,1071,1141};
const int Cyc_lex_backtrk[406]=(const int[406]){- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,2,- 1,8,- 1,3,5,- 1,- 1,6,5,- 1,- 1,- 1,6,- 1,5,1,0,
- 1,0,1,- 1,12,13,- 1,13,13,13,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,11,12,2,4,4,- 1,0,0,0,2,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,2,8,3,5,- 1,6,5,
6,5,2,8,3,5,- 1,6,5,- 1,20,20,20,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,14,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,16,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,1,9,2,4,- 1,5,
4,- 1,- 1,2,- 1,48,- 1,48,48,48,48,48,48,48,48,5,7,48,48,48,48,0,48,48,- 1,- 1,- 1,0,- 1,
43,- 1,- 1,42,- 1,- 1,- 1,- 1,9,7,- 1,7,7,- 1,8,9,- 1,- 1,9,5,6,5,5,- 1,4,4,4,6,6,5,5,- 1,- 1,
- 1,9,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,1,2,1,1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};const int Cyc_lex_default[
406]=(const int[406]){- 1,- 1,- 1,297,286,138,23,36,23,19,- 1,- 1,12,31,46,31,36,23,19,
0,- 1,0,0,0,0,- 1,0,- 1,0,- 1,- 1,0,- 1,- 1,- 1,0,0,0,- 1,0,40,- 1,- 1,0,- 1,- 1,0,- 1,- 1,0,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,103,- 1,- 1,
- 1,- 1,- 1,110,110,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,-
1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,0,- 1,0,- 1,0,- 1,- 1,366,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,0,
- 1,0,0,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,
0,0,0,- 1,- 1,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};const int Cyc_lex_trans[
3481]=(const int[3481]){0,0,0,0,0,0,0,0,0,0,22,19,28,398,19,28,19,28,100,19,45,45,
45,45,45,22,45,0,0,0,0,0,21,202,213,399,21,22,- 1,- 1,22,- 1,- 1,45,203,45,204,22,
396,396,396,396,396,396,396,396,396,396,31,103,22,214,114,40,227,396,396,396,396,
396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,
396,396,31,245,240,235,396,130,396,396,396,396,396,396,396,396,396,396,396,396,
396,396,396,396,396,396,396,396,396,396,396,396,396,396,394,394,394,394,394,394,
394,394,394,394,121,20,74,67,54,55,56,394,394,394,394,394,394,394,394,394,394,
394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,57,58,59,60,394,
61,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,
394,394,394,394,394,394,394,62,63,37,298,299,298,298,299,39,22,64,65,68,69,70,
129,34,34,34,34,34,34,34,34,71,72,298,300,301,75,76,302,303,304,104,104,305,306,
104,307,308,309,310,311,311,311,311,311,311,311,311,311,312,77,313,314,315,104,
19,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,
316,316,316,316,316,316,316,19,- 1,- 1,317,316,101,316,316,316,316,316,316,316,316,
316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,287,318,
37,288,139,139,24,24,139,131,122,94,84,81,85,38,82,86,28,87,24,29,83,25,289,88,
95,139,96,22,26,26,21,21,115,116,117,140,118,119,120,142,191,192,26,35,35,35,35,
35,35,35,35,177,171,162,30,30,30,30,30,30,30,30,66,66,156,149,66,73,73,78,78,73,
150,78,151,152,153,154,66,66,290,155,66,157,158,66,132,123,141,39,73,22,78,159,
143,144,145,160,146,161,27,66,147,31,46,21,73,73,163,164,73,148,165,166,167,113,
113,113,113,113,113,113,113,113,113,- 1,32,- 1,- 1,73,- 1,22,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
- 1,168,- 1,- 1,113,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,89,89,28,169,89,170,100,172,173,174,
97,97,104,104,97,175,104,112,112,176,178,112,179,89,180,105,105,80,80,105,19,80,
21,97,181,104,182,183,184,185,112,186,187,188,189,190,251,193,105,194,80,195,112,
112,91,196,112,197,198,21,21,21,106,107,106,106,106,106,106,106,106,106,106,106,
21,112,199,200,201,205,206,106,106,106,106,106,106,106,106,106,106,106,106,106,
106,106,106,106,106,106,106,106,106,106,106,106,106,207,208,209,210,106,211,106,
106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,
106,106,106,106,106,47,47,35,212,47,215,216,33,33,33,33,33,33,33,33,33,33,92,92,
230,222,92,223,47,33,33,33,33,33,33,128,128,128,128,128,128,128,128,48,224,92,
225,- 1,226,- 1,228,229,99,99,231,49,99,232,233,234,93,33,33,33,33,33,33,35,35,35,
35,35,35,35,35,99,236,237,238,239,- 1,217,- 1,41,41,218,102,41,101,101,241,242,101,
243,219,50,220,244,91,246,51,52,247,248,249,250,41,53,252,253,268,101,137,137,
137,137,137,137,137,137,263,259,42,42,42,42,42,42,42,42,42,42,221,260,261,262,93,
36,28,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,264,100,265,266,42,267,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,42,269,270,43,271,272,102,281,33,33,33,33,33,33,33,33,33,33,
276,277,278,279,280,282,283,33,33,33,33,33,33,35,35,35,35,35,35,35,35,284,285,
392,391,365,356,331,44,44,44,44,44,44,44,44,44,44,21,33,33,33,33,33,33,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,327,328,329,
326,44,321,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,323,324,325,28,330,359,100,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,388,254,155,138,
44,100,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,78,78,389,363,78,- 1,253,253,190,102,253,295,295,295,295,295,295,295,295,
319,250,364,100,78,229,390,46,273,273,253,384,273,255,255,- 1,360,255,254,176,79,
79,79,79,79,79,79,79,79,79,383,273,361,362,35,100,255,79,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,320,385,21,35,79,367,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,31,80,80,
274,400,80,267,267,256,401,267,402,393,275,393,393,403,404,257,35,31,31,36,258,
80,405,21,255,255,267,0,255,273,273,0,393,273,19,0,0,79,79,79,79,79,79,79,79,79,
79,255,35,31,31,36,273,36,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,0,0,0,355,79,36,79,79,79,79,79,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,89,89,256,355,89,272,272,274,0,272,296,
296,257,0,296,0,0,275,355,0,0,0,- 1,89,0,0,296,296,272,393,296,393,393,296,0,21,
31,0,0,90,90,90,90,90,90,90,90,90,90,296,355,21,393,0,0,0,90,90,90,90,90,90,90,
90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,0,0,0,0,90,0,90,90,90,
90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,91,92,92,
280,280,92,0,280,285,285,0,0,285,31,31,31,31,31,31,31,31,0,0,0,92,0,280,0,0,0,0,
285,0,0,28,0,0,0,0,35,90,90,90,90,90,90,90,90,90,90,0,0,0,0,0,0,0,90,90,90,90,90,
90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,0,0,0,0,90,0,90,
90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,97,97,
0,357,97,358,358,358,358,358,358,358,358,358,358,0,0,0,0,337,0,337,0,97,338,338,
338,338,338,338,338,338,338,338,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,0,0,0,0,
0,0,0,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,
98,0,0,0,0,98,0,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,
98,98,98,98,98,36,99,99,0,0,99,338,338,338,338,338,338,338,338,338,338,0,0,0,0,
341,0,341,0,99,342,342,342,342,342,342,342,342,342,342,0,0,0,0,0,98,98,98,98,98,
98,98,98,98,98,0,0,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,
98,98,98,98,98,98,98,98,98,0,0,0,0,98,0,98,98,98,98,98,98,98,98,98,98,98,98,98,
98,98,98,98,98,98,98,98,98,98,98,98,98,108,108,108,108,108,108,108,108,108,108,
108,108,22,0,0,0,0,0,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,0,0,0,0,108,0,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,109,108,108,108,108,108,108,108,108,108,108,22,0,0,0,0,0,0,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,0,0,0,0,108,0,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,22,0,0,0,0,0,0,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,- 1,0,0,- 1,108,0,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,0,0,0,0,109,109,109,109,109,109,109,109,109,109,109,109,
111,0,0,0,0,0,0,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,
109,109,109,109,109,109,109,109,109,109,0,0,0,0,109,0,109,109,109,109,109,109,
109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,
113,113,113,113,113,113,113,113,113,113,0,0,0,0,0,0,0,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
28,0,0,124,113,0,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,0,0,28,0,0,133,125,125,125,125,125,125,
125,125,127,127,127,127,127,127,127,127,127,127,0,0,0,0,0,0,0,127,127,127,127,
127,127,0,0,0,134,134,134,134,134,134,134,134,0,0,0,0,0,0,0,31,0,0,- 1,0,385,0,0,
127,127,127,127,127,127,386,386,386,386,386,386,386,386,0,126,127,127,127,127,
127,127,127,127,127,127,31,0,0,0,0,0,0,127,127,127,127,127,127,136,136,136,136,
136,136,136,136,136,136,135,0,0,0,0,0,0,136,136,136,136,136,136,0,0,0,127,127,
127,127,127,127,19,0,0,291,0,0,136,136,136,136,136,136,136,136,136,136,0,136,136,
136,136,136,136,136,136,136,136,136,136,0,0,0,0,0,0,0,0,385,292,292,292,292,292,
292,292,292,387,387,387,387,387,387,387,387,0,136,136,136,136,136,136,0,294,294,
294,294,294,294,294,294,294,294,0,0,0,0,0,0,0,294,294,294,294,294,294,0,0,28,342,
342,342,342,342,342,342,342,342,342,0,0,294,294,294,294,294,294,294,294,294,294,
293,294,294,294,294,294,294,294,294,294,294,294,294,0,0,332,0,343,343,343,343,
343,343,343,343,344,344,0,0,0,0,0,0,0,0,0,0,0,334,294,294,294,294,294,294,345,0,
0,0,0,0,0,0,0,346,0,0,347,332,0,333,333,333,333,333,333,333,333,333,333,334,0,0,
0,0,0,0,345,0,0,0,334,0,0,0,0,346,0,335,347,0,0,0,0,0,0,0,336,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,334,0,0,0,0,0,0,335,0,0,0,0,0,0,0,0,336,322,322,322,322,322,322,322,
322,322,322,0,0,0,0,0,0,0,322,322,322,322,322,322,322,322,322,322,322,322,322,
322,322,322,322,322,322,322,322,322,322,322,322,322,0,0,0,0,322,0,322,322,322,
322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,
322,322,322,322,322,322,322,322,322,322,322,322,322,0,0,0,0,0,0,0,322,322,322,
322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,
322,322,322,0,0,0,0,322,0,322,322,322,322,322,322,322,322,322,322,322,322,322,
322,322,322,322,322,322,322,322,322,322,322,322,322,339,339,339,339,339,339,339,
339,339,339,0,0,0,0,0,0,0,0,0,0,0,340,93,0,0,0,0,332,93,333,333,333,333,333,333,
333,333,333,333,338,338,338,338,338,338,338,338,338,338,0,334,0,0,340,93,0,0,335,
0,0,93,91,0,0,0,0,336,91,0,339,339,339,339,339,339,339,339,339,339,0,0,0,334,0,0,
0,0,0,0,335,340,93,0,91,0,0,0,93,336,91,0,0,342,342,342,342,342,342,342,342,342,
342,0,0,0,0,0,0,0,0,0,0,340,93,93,0,0,0,0,93,93,332,0,343,343,343,343,343,343,
343,343,344,344,0,0,0,0,0,0,0,0,0,0,0,334,0,93,0,0,0,0,353,93,0,0,0,0,0,0,332,
354,344,344,344,344,344,344,344,344,344,344,0,0,0,0,0,334,0,0,0,0,0,334,353,0,0,
0,0,0,351,0,0,354,0,0,0,0,0,352,0,0,358,358,358,358,358,358,358,358,358,358,0,0,
0,334,0,0,0,0,0,0,351,340,93,0,0,0,0,0,93,352,348,348,348,348,348,348,348,348,
348,348,0,0,0,0,0,0,0,348,348,348,348,348,348,340,93,0,0,0,0,0,93,0,0,0,0,0,0,0,
348,348,348,348,348,348,348,348,348,348,0,348,348,348,348,348,348,348,348,348,
348,348,348,0,0,0,368,0,349,0,0,369,0,0,0,0,0,350,0,0,370,370,370,370,370,370,
370,370,0,348,348,348,348,348,348,371,0,0,0,0,349,0,0,0,0,0,0,0,0,350,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,372,0,0,0,0,373,374,0,0,0,375,0,0,0,0,0,0,0,376,0,0,0,377,0,
378,0,379,0,380,381,381,381,381,381,381,381,381,381,381,0,0,0,0,0,0,0,381,381,
381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,
381,381,381,381,0,0,0,0,0,0,381,381,381,381,381,381,381,381,381,381,381,381,381,
381,381,381,381,381,381,381,381,381,381,381,381,381,382,0,0,0,0,0,0,0,0,381,381,
381,381,381,381,381,381,381,381,0,0,0,0,0,0,0,381,381,381,381,381,381,381,381,
381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,0,0,0,0,
0,0,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,
381,381,381,381,381,381,381,19,0,0,395,0,0,0,394,394,394,394,394,394,394,394,394,
394,0,0,0,0,0,0,0,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,
394,394,394,394,394,394,394,394,394,394,394,0,0,0,0,394,0,394,394,394,394,394,
394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,
394,397,0,0,0,0,0,0,0,396,396,396,396,396,396,396,396,396,396,0,0,0,0,0,0,0,396,
396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,
396,396,396,396,396,0,0,0,0,396,0,396,396,396,396,396,396,396,396,396,396,396,
396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0};const int Cyc_lex_check[3481]=(const int[3481]){- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,0,25,29,0,122,124,131,133,288,291,41,41,45,45,41,398,45,- 1,- 1,- 1,- 1,- 1,120,
201,212,0,395,10,12,40,10,12,40,41,202,45,203,20,1,1,1,1,1,1,1,1,1,1,38,48,121,
213,10,38,226,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,129,217,218,
219,1,129,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,
2,9,18,51,52,53,54,55,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,56,57,
58,59,2,60,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,61,62,7,3,3,3,3,3,
7,7,63,64,67,68,69,7,30,30,30,30,30,30,30,30,70,71,3,3,3,74,75,3,3,3,47,47,3,3,
47,3,3,3,3,3,3,3,3,3,3,3,3,3,3,76,3,3,3,47,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,10,12,40,3,3,81,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,4,3,16,4,5,5,6,8,5,6,8,82,83,50,84,16,50,85,27,86,17,27,50,17,4,87,94,5,95,5,
6,8,9,18,114,115,116,5,117,118,119,141,143,143,17,34,34,34,34,34,34,34,34,144,
145,146,27,27,27,27,27,27,27,27,65,65,147,148,65,72,72,77,77,72,149,77,150,151,
152,153,66,66,4,154,66,156,157,65,6,8,5,16,72,16,77,158,142,142,142,159,142,160,
17,66,142,27,161,7,73,73,162,163,73,142,164,165,166,11,11,11,11,11,11,11,11,11,
11,103,27,110,103,73,110,3,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,11,11,111,167,130,111,11,130,11,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,88,88,66,168,88,169,170,171,172,173,
96,96,104,104,96,174,104,105,105,175,177,105,178,88,179,13,13,80,80,13,73,80,16,
96,180,104,181,182,183,184,105,185,186,187,188,189,191,192,13,193,80,194,112,112,
4,195,112,196,197,5,6,8,13,13,13,13,13,13,13,13,13,13,13,13,17,112,198,199,200,
204,205,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,206,207,208,209,13,210,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,13,13,13,13,13,13,13,14,14,80,211,14,214,215,32,32,32,32,32,32,32,32,32,32,
92,92,220,221,92,222,14,32,32,32,32,32,32,125,125,125,125,125,125,125,125,14,223,
92,224,103,225,110,227,228,99,99,230,14,99,231,232,233,234,32,32,32,32,32,32,128,
128,128,128,128,128,128,128,99,235,236,237,238,111,216,130,15,15,216,239,15,101,
101,240,241,101,242,216,14,216,243,244,245,14,14,246,247,248,249,15,14,251,252,
256,101,134,134,134,134,134,134,134,134,257,258,15,15,15,15,15,15,15,15,15,15,
216,259,260,261,92,262,13,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
15,15,15,15,15,15,15,15,263,99,264,265,15,266,15,15,15,15,15,15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,268,269,15,270,271,101,274,33,33,33,
33,33,33,33,33,33,33,275,276,277,278,279,281,282,33,33,33,33,33,33,137,137,137,
137,137,137,137,137,283,284,300,302,305,309,312,42,42,42,42,42,42,42,42,42,42,14,
33,33,33,33,33,33,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,313,313,313,314,42,317,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,42,42,42,42,42,44,44,44,44,44,44,44,44,44,44,315,315,324,15,
327,357,335,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,366,368,369,371,44,335,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,78,78,303,306,78,304,253,253,372,373,253,292,292,
292,292,292,292,292,292,318,374,306,336,78,375,303,376,139,139,253,377,139,140,
140,304,307,140,253,378,78,78,78,78,78,78,78,78,78,78,379,139,307,307,345,336,
140,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,
78,318,387,397,345,78,304,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,
78,78,78,78,78,78,78,78,78,79,79,139,399,79,267,267,140,400,267,401,298,139,298,
298,402,403,140,346,349,350,351,140,79,404,405,255,255,267,- 1,255,273,273,- 1,298,
273,267,- 1,- 1,79,79,79,79,79,79,79,79,79,79,255,346,349,350,351,273,352,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,- 1,- 1,- 1,
353,79,352,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,
79,79,79,89,89,255,353,89,272,272,273,- 1,272,289,289,255,- 1,289,- 1,- 1,273,354,- 1,
- 1,- 1,304,89,- 1,- 1,296,296,272,393,296,393,393,289,- 1,289,272,- 1,- 1,89,89,89,89,
89,89,89,89,89,89,296,354,296,393,- 1,- 1,- 1,89,89,89,89,89,89,89,89,89,89,89,89,
89,89,89,89,89,89,89,89,89,89,89,89,89,89,- 1,- 1,- 1,- 1,89,- 1,89,89,89,89,89,89,89,
89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,90,90,280,280,90,- 1,
280,285,285,- 1,- 1,285,295,295,295,295,295,295,295,295,- 1,- 1,- 1,90,- 1,280,- 1,- 1,-
1,- 1,285,- 1,- 1,280,- 1,- 1,- 1,- 1,285,90,90,90,90,90,90,90,90,90,90,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,
90,- 1,- 1,- 1,- 1,90,- 1,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,
90,90,90,90,90,90,97,97,- 1,308,97,308,308,308,308,308,308,308,308,308,308,- 1,- 1,
- 1,- 1,334,- 1,334,- 1,97,334,334,334,334,334,334,334,334,334,334,- 1,- 1,- 1,- 1,- 1,97,
97,97,97,97,97,97,97,97,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,97,97,97,97,97,97,97,97,97,97,
97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,- 1,- 1,- 1,- 1,97,- 1,97,97,97,97,97,97,
97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,98,98,- 1,- 1,98,
337,337,337,337,337,337,337,337,337,337,- 1,- 1,- 1,- 1,340,- 1,340,- 1,98,340,340,340,
340,340,340,340,340,340,340,- 1,- 1,- 1,- 1,- 1,98,98,98,98,98,98,98,98,98,98,- 1,- 1,-
1,- 1,- 1,- 1,- 1,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,
98,98,98,98,- 1,- 1,- 1,- 1,98,- 1,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,
98,98,98,98,98,98,98,98,98,106,106,106,106,106,106,106,106,106,106,106,106,106,-
1,- 1,- 1,- 1,- 1,- 1,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,
106,106,106,106,106,106,106,106,106,106,- 1,- 1,- 1,- 1,106,- 1,106,106,106,106,106,
106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,
106,107,107,107,107,107,107,107,107,107,107,107,107,107,- 1,- 1,- 1,- 1,- 1,- 1,107,
107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,
107,107,107,107,107,- 1,- 1,- 1,- 1,107,- 1,107,107,107,107,107,107,107,107,107,107,
107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,108,108,108,108,
108,108,108,108,108,108,108,108,108,- 1,- 1,- 1,- 1,- 1,- 1,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
109,- 1,- 1,109,108,- 1,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,- 1,- 1,- 1,- 1,109,109,109,109,109,109,
109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,- 1,- 1,109,109,109,109,109,109,109,109,
109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,
- 1,109,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,
109,109,109,109,109,109,109,109,109,113,113,113,113,113,113,113,113,113,113,- 1,-
1,- 1,- 1,- 1,- 1,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,123,- 1,- 1,123,113,- 1,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,- 1,- 1,132,- 1,- 1,132,123,123,123,123,123,123,123,123,126,126,126,126,126,126,
126,126,126,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,126,126,126,126,126,- 1,- 1,- 1,132,132,
132,132,132,132,132,132,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,- 1,- 1,109,- 1,370,- 1,- 1,126,126,
126,126,126,126,370,370,370,370,370,370,370,370,- 1,123,127,127,127,127,127,127,
127,127,127,127,132,- 1,- 1,- 1,- 1,- 1,- 1,127,127,127,127,127,127,135,135,135,135,
135,135,135,135,135,135,132,- 1,- 1,- 1,- 1,- 1,- 1,135,135,135,135,135,135,- 1,- 1,- 1,
127,127,127,127,127,127,290,- 1,- 1,290,- 1,- 1,136,136,136,136,136,136,136,136,136,
136,- 1,135,135,135,135,135,135,136,136,136,136,136,136,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
386,290,290,290,290,290,290,290,290,386,386,386,386,386,386,386,386,- 1,136,136,
136,136,136,136,- 1,293,293,293,293,293,293,293,293,293,293,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
293,293,293,293,293,293,- 1,- 1,290,341,341,341,341,341,341,341,341,341,341,- 1,- 1,
294,294,294,294,294,294,294,294,294,294,290,293,293,293,293,293,293,294,294,294,
294,294,294,- 1,- 1,310,- 1,310,310,310,310,310,310,310,310,310,310,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,310,294,294,294,294,294,294,310,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,310,- 1,
- 1,310,311,- 1,311,311,311,311,311,311,311,311,311,311,310,- 1,- 1,- 1,- 1,- 1,- 1,310,
- 1,- 1,- 1,311,- 1,- 1,- 1,- 1,310,- 1,311,310,- 1,- 1,- 1,- 1,- 1,- 1,- 1,311,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,311,- 1,- 1,- 1,- 1,- 1,- 1,311,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
311,316,316,316,316,316,316,316,316,316,316,- 1,- 1,- 1,- 1,- 1,- 1,- 1,316,316,316,316,
316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,
316,316,- 1,- 1,- 1,- 1,316,- 1,316,316,316,316,316,316,316,316,316,316,316,316,316,
316,316,316,316,316,316,316,316,316,316,316,316,316,322,322,322,322,322,322,322,
322,322,322,- 1,- 1,- 1,- 1,- 1,- 1,- 1,322,322,322,322,322,322,322,322,322,322,322,322,
322,322,322,322,322,322,322,322,322,322,322,322,322,322,- 1,- 1,- 1,- 1,322,- 1,322,
322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,
322,322,322,322,322,332,332,332,332,332,332,332,332,332,332,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,332,332,- 1,- 1,- 1,- 1,333,332,333,333,333,333,333,333,333,333,333,333,
338,338,338,338,338,338,338,338,338,338,- 1,333,- 1,- 1,332,332,- 1,- 1,333,- 1,- 1,332,
338,- 1,- 1,- 1,- 1,333,338,- 1,339,339,339,339,339,339,339,339,339,339,- 1,- 1,- 1,333,
- 1,- 1,- 1,- 1,- 1,- 1,333,339,339,- 1,338,- 1,- 1,- 1,339,333,338,- 1,- 1,342,342,342,342,
342,342,342,342,342,342,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,339,339,342,- 1,- 1,- 1,- 1,
339,342,343,- 1,343,343,343,343,343,343,343,343,343,343,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,343,- 1,342,- 1,- 1,- 1,- 1,343,342,- 1,- 1,- 1,- 1,- 1,- 1,344,343,344,344,344,344,
344,344,344,344,344,344,- 1,- 1,- 1,- 1,- 1,343,- 1,- 1,- 1,- 1,- 1,344,343,- 1,- 1,- 1,- 1,- 1,
344,- 1,- 1,343,- 1,- 1,- 1,- 1,- 1,344,- 1,- 1,358,358,358,358,358,358,358,358,358,358,-
1,- 1,- 1,344,- 1,- 1,- 1,- 1,- 1,- 1,344,358,358,- 1,- 1,- 1,- 1,- 1,358,344,347,347,347,347,
347,347,347,347,347,347,- 1,- 1,- 1,- 1,- 1,- 1,- 1,347,347,347,347,347,347,358,358,- 1,
- 1,- 1,- 1,- 1,358,- 1,- 1,- 1,- 1,- 1,- 1,- 1,348,348,348,348,348,348,348,348,348,348,- 1,
347,347,347,347,347,347,348,348,348,348,348,348,- 1,- 1,- 1,367,- 1,348,- 1,- 1,367,- 1,
- 1,- 1,- 1,- 1,348,- 1,- 1,367,367,367,367,367,367,367,367,- 1,348,348,348,348,348,348,
367,- 1,- 1,- 1,- 1,348,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,348,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,367,- 1,- 1,- 1,- 1,367,367,- 1,- 1,- 1,367,- 1,- 1,- 1,- 1,- 1,- 1,- 1,367,- 1,- 1,- 1,
367,- 1,367,- 1,367,- 1,367,380,380,380,380,380,380,380,380,380,380,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,
380,380,380,380,380,380,380,380,- 1,- 1,- 1,- 1,- 1,- 1,380,380,380,380,380,380,380,
380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,381,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,381,381,381,381,381,381,381,381,381,381,- 1,- 1,- 1,- 1,- 1,-
1,- 1,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,
381,381,381,381,381,381,381,- 1,- 1,- 1,- 1,- 1,- 1,381,381,381,381,381,381,381,381,
381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,394,- 1,-
1,394,- 1,- 1,- 1,394,394,394,394,394,394,394,394,394,394,- 1,- 1,- 1,- 1,- 1,- 1,- 1,394,
394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,
394,394,394,394,394,- 1,- 1,- 1,- 1,394,- 1,394,394,394,394,394,394,394,394,394,394,
394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,396,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,396,396,396,396,396,396,396,396,396,396,- 1,- 1,- 1,- 1,- 1,- 1,- 1,396,396,
396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,
396,396,396,396,- 1,- 1,- 1,- 1,396,- 1,396,396,396,396,396,396,396,396,396,396,396,
396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};int Cyc_lex_engine(int
start_state,struct Cyc_Lexing_lexbuf*lbuf);int Cyc_lex_engine(int start_state,
struct Cyc_Lexing_lexbuf*lbuf){int state;int base;int backtrk;int c;state=start_state;
if(state >= 0){lbuf->lex_last_pos=(lbuf->lex_start_pos=lbuf->lex_curr_pos);lbuf->lex_last_action=
- 1;}else{state=(- state)- 1;}while(1){base=Cyc_lex_base[
_check_known_subscript_notnull(406,state)];if(base < 0)return(- base)- 1;backtrk=
Cyc_lex_backtrk[_check_known_subscript_notnull(406,state)];if(backtrk >= 0){lbuf->lex_last_pos=
lbuf->lex_curr_pos;lbuf->lex_last_action=backtrk;}if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)return(- state)- 1;else{c=256;}}else{c=(int)*((char*)
_check_dyneither_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c == - 1)c=256;}if(Cyc_lex_check[_check_known_subscript_notnull(3481,base + c)]
== state)state=Cyc_lex_trans[_check_known_subscript_notnull(3481,base + c)];else{
state=Cyc_lex_default[_check_known_subscript_notnull(406,state)];}if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;if(lbuf->lex_last_action == - 1){struct Cyc_Lexing_Error_struct
_tmp3AC;const char*_tmp3AB;struct Cyc_Lexing_Error_struct*_tmp3AA;(int)_throw((
void*)((_tmp3AA=_cycalloc(sizeof(*_tmp3AA)),((_tmp3AA[0]=((_tmp3AC.tag=Cyc_Lexing_Error,((
_tmp3AC.f1=((_tmp3AB="empty token",_tag_dyneither(_tmp3AB,sizeof(char),12))),
_tmp3AC)))),_tmp3AA)))));}else{return lbuf->lex_last_action;}}else{if(c == 256)
lbuf->lex_eof_reached=0;}}}struct _tuple17*Cyc_line_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate);struct _tuple17*Cyc_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,
int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0:
_LL0: Cyc_macroname(lexbuf);for(0;Cyc_current_args != 0;Cyc_current_args=((struct
Cyc_List_List*)_check_null(Cyc_current_args))->tl){struct Cyc_Set_Set**_tmp3AD;
Cyc_current_targets=((_tmp3AD=_cycalloc(sizeof(*_tmp3AD)),((_tmp3AD[0]=((struct
Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_delete)(*((
struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),(struct _dyneither_ptr*)((
struct Cyc_List_List*)_check_null(Cyc_current_args))->hd),_tmp3AD))));}{struct
_tuple17*_tmp3AE;return(_tmp3AE=_cycalloc(sizeof(*_tmp3AE)),((_tmp3AE->f1=(
struct _dyneither_ptr*)_check_null(Cyc_current_source),((_tmp3AE->f2=*((struct Cyc_Set_Set**)
_check_null(Cyc_current_targets)),_tmp3AE)))));};case 1: _LL1: return Cyc_line(
lexbuf);case 2: _LL2: return 0;default: _LL3:(lexbuf->refill_buff)(lexbuf);return Cyc_line_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp3B4;const char*_tmp3B3;struct
Cyc_Lexing_Error_struct*_tmp3B2;(int)_throw((void*)((_tmp3B2=_cycalloc(sizeof(*
_tmp3B2)),((_tmp3B2[0]=((_tmp3B4.tag=Cyc_Lexing_Error,((_tmp3B4.f1=((_tmp3B3="some action didn't return!",
_tag_dyneither(_tmp3B3,sizeof(char),27))),_tmp3B4)))),_tmp3B2)))));};}struct
_tuple17*Cyc_line(struct Cyc_Lexing_lexbuf*lexbuf);struct _tuple17*Cyc_line(struct
Cyc_Lexing_lexbuf*lexbuf){return Cyc_line_rec(lexbuf,0);}int Cyc_macroname_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_macroname_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL5:{struct _dyneither_ptr*_tmp3B5;Cyc_current_source=((_tmp3B5=_cycalloc(
sizeof(*_tmp3B5)),((_tmp3B5[0]=(struct _dyneither_ptr)Cyc_substring((struct
_dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2)),_tmp3B5))));}Cyc_current_args=0;{
struct Cyc_Set_Set**_tmp3B6;Cyc_current_targets=((_tmp3B6=_cycalloc(sizeof(*
_tmp3B6)),((_tmp3B6[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp3B6))));}Cyc_token(
lexbuf);return 0;case 1: _LL6:{struct _dyneither_ptr*_tmp3B7;Cyc_current_source=((
_tmp3B7=_cycalloc(sizeof(*_tmp3B7)),((_tmp3B7[0]=(struct _dyneither_ptr)Cyc_substring((
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1)),_tmp3B7))));}Cyc_current_args=0;{
struct Cyc_Set_Set**_tmp3B8;Cyc_current_targets=((_tmp3B8=_cycalloc(sizeof(*
_tmp3B8)),((_tmp3B8[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp3B8))));}Cyc_args(
lexbuf);return 0;case 2: _LL7:{struct _dyneither_ptr*_tmp3B9;Cyc_current_source=((
_tmp3B9=_cycalloc(sizeof(*_tmp3B9)),((_tmp3B9[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf),_tmp3B9))));}Cyc_current_args=0;{struct Cyc_Set_Set**_tmp3BA;Cyc_current_targets=((
_tmp3BA=_cycalloc(sizeof(*_tmp3BA)),((_tmp3BA[0]=((struct Cyc_Set_Set*(*)(int(*
cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),
_tmp3BA))));}Cyc_token(lexbuf);return 0;default: _LL8:(lexbuf->refill_buff)(lexbuf);
return Cyc_macroname_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp3C0;
const char*_tmp3BF;struct Cyc_Lexing_Error_struct*_tmp3BE;(int)_throw((void*)((
_tmp3BE=_cycalloc(sizeof(*_tmp3BE)),((_tmp3BE[0]=((_tmp3C0.tag=Cyc_Lexing_Error,((
_tmp3C0.f1=((_tmp3BF="some action didn't return!",_tag_dyneither(_tmp3BF,sizeof(
char),27))),_tmp3C0)))),_tmp3BE)))));};}int Cyc_macroname(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_macroname_rec(
lexbuf,1);}int Cyc_args_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_args_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LLA: {struct _dyneither_ptr*_tmp3C1;struct
_dyneither_ptr*_tmp15=(_tmp3C1=_cycalloc(sizeof(*_tmp3C1)),((_tmp3C1[0]=(struct
_dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(
unsigned long)((Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 
2)),_tmp3C1)));{struct Cyc_List_List*_tmp3C2;Cyc_current_args=((_tmp3C2=_cycalloc(
sizeof(*_tmp3C2)),((_tmp3C2->hd=_tmp15,((_tmp3C2->tl=Cyc_current_args,_tmp3C2))))));}
return Cyc_args(lexbuf);}case 1: _LLB: {struct _dyneither_ptr*_tmp3C3;struct
_dyneither_ptr*_tmp18=(_tmp3C3=_cycalloc(sizeof(*_tmp3C3)),((_tmp3C3[0]=(struct
_dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(
unsigned long)((Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 
1)),_tmp3C3)));{struct Cyc_List_List*_tmp3C4;Cyc_current_args=((_tmp3C4=_cycalloc(
sizeof(*_tmp3C4)),((_tmp3C4->hd=_tmp18,((_tmp3C4->tl=Cyc_current_args,_tmp3C4))))));}
return Cyc_args(lexbuf);}case 2: _LLC: {struct _dyneither_ptr*_tmp3C5;struct
_dyneither_ptr*_tmp1B=(_tmp3C5=_cycalloc(sizeof(*_tmp3C5)),((_tmp3C5[0]=(struct
_dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(
unsigned long)((Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 
1)),_tmp3C5)));{struct Cyc_List_List*_tmp3C6;Cyc_current_args=((_tmp3C6=_cycalloc(
sizeof(*_tmp3C6)),((_tmp3C6->hd=_tmp1B,((_tmp3C6->tl=Cyc_current_args,_tmp3C6))))));}
return Cyc_token(lexbuf);}default: _LLD:(lexbuf->refill_buff)(lexbuf);return Cyc_args_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp3CC;const char*_tmp3CB;struct
Cyc_Lexing_Error_struct*_tmp3CA;(int)_throw((void*)((_tmp3CA=_cycalloc(sizeof(*
_tmp3CA)),((_tmp3CA[0]=((_tmp3CC.tag=Cyc_Lexing_Error,((_tmp3CC.f1=((_tmp3CB="some action didn't return!",
_tag_dyneither(_tmp3CB,sizeof(char),27))),_tmp3CC)))),_tmp3CA)))));};}int Cyc_args(
struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_args(struct Cyc_Lexing_lexbuf*lexbuf){
return Cyc_args_rec(lexbuf,2);}int Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,
int lexstate);int Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLF:{struct
_dyneither_ptr*_tmp3CD;Cyc_add_target(((_tmp3CD=_cycalloc(sizeof(*_tmp3CD)),((
_tmp3CD[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),_tmp3CD)))));}return
Cyc_token(lexbuf);case 1: _LL10: return 0;case 2: _LL11: return Cyc_token(lexbuf);case 3:
_LL12: Cyc_string(lexbuf);return Cyc_token(lexbuf);case 4: _LL13: return Cyc_token(
lexbuf);case 5: _LL14: return Cyc_token(lexbuf);case 6: _LL15: return Cyc_token(lexbuf);
case 7: _LL16: return Cyc_token(lexbuf);case 8: _LL17: return Cyc_token(lexbuf);case 9:
_LL18: return Cyc_token(lexbuf);case 10: _LL19: return Cyc_token(lexbuf);case 11: _LL1A:
return Cyc_token(lexbuf);case 12: _LL1B: return Cyc_token(lexbuf);case 13: _LL1C: return
Cyc_token(lexbuf);case 14: _LL1D: return Cyc_token(lexbuf);case 15: _LL1E: return Cyc_token(
lexbuf);case 16: _LL1F: return Cyc_token(lexbuf);case 17: _LL20: return Cyc_token(lexbuf);
case 18: _LL21: return Cyc_token(lexbuf);case 19: _LL22: return Cyc_token(lexbuf);case 20:
_LL23: return Cyc_token(lexbuf);case 21: _LL24: return Cyc_token(lexbuf);case 22: _LL25:
return Cyc_token(lexbuf);case 23: _LL26: return Cyc_token(lexbuf);case 24: _LL27: return
Cyc_token(lexbuf);case 25: _LL28: return Cyc_token(lexbuf);case 26: _LL29: return Cyc_token(
lexbuf);case 27: _LL2A: return Cyc_token(lexbuf);case 28: _LL2B: return Cyc_token(lexbuf);
case 29: _LL2C: return Cyc_token(lexbuf);case 30: _LL2D: return Cyc_token(lexbuf);case 31:
_LL2E: return Cyc_token(lexbuf);case 32: _LL2F: return Cyc_token(lexbuf);case 33: _LL30:
return Cyc_token(lexbuf);case 34: _LL31: return Cyc_token(lexbuf);case 35: _LL32: return
Cyc_token(lexbuf);case 36: _LL33: return Cyc_token(lexbuf);case 37: _LL34: return Cyc_token(
lexbuf);case 38: _LL35: return Cyc_token(lexbuf);case 39: _LL36: return Cyc_token(lexbuf);
case 40: _LL37: return Cyc_token(lexbuf);case 41: _LL38: return Cyc_token(lexbuf);case 42:
_LL39: return Cyc_token(lexbuf);case 43: _LL3A: return Cyc_token(lexbuf);case 44: _LL3B:
return Cyc_token(lexbuf);case 45: _LL3C: return Cyc_token(lexbuf);case 46: _LL3D: return
Cyc_token(lexbuf);case 47: _LL3E: return Cyc_token(lexbuf);case 48: _LL3F: return Cyc_token(
lexbuf);default: _LL40:(lexbuf->refill_buff)(lexbuf);return Cyc_token_rec(lexbuf,
lexstate);}{struct Cyc_Lexing_Error_struct _tmp3D3;const char*_tmp3D2;struct Cyc_Lexing_Error_struct*
_tmp3D1;(int)_throw((void*)((_tmp3D1=_cycalloc(sizeof(*_tmp3D1)),((_tmp3D1[0]=((
_tmp3D3.tag=Cyc_Lexing_Error,((_tmp3D3.f1=((_tmp3D2="some action didn't return!",
_tag_dyneither(_tmp3D2,sizeof(char),27))),_tmp3D3)))),_tmp3D1)))));};}int Cyc_token(
struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){
return Cyc_token_rec(lexbuf,3);}int Cyc_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,
int lexstate);int Cyc_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL42: return Cyc_string(
lexbuf);case 1: _LL43: return 0;case 2: _LL44: return Cyc_string(lexbuf);case 3: _LL45:
return Cyc_string(lexbuf);case 4: _LL46: return Cyc_string(lexbuf);case 5: _LL47: return
Cyc_string(lexbuf);case 6: _LL48: return Cyc_string(lexbuf);case 7: _LL49: return 0;case
8: _LL4A: return 0;case 9: _LL4B: return Cyc_string(lexbuf);default: _LL4C:(lexbuf->refill_buff)(
lexbuf);return Cyc_string_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp3D9;const char*_tmp3D8;struct Cyc_Lexing_Error_struct*_tmp3D7;(int)_throw((
void*)((_tmp3D7=_cycalloc(sizeof(*_tmp3D7)),((_tmp3D7[0]=((_tmp3D9.tag=Cyc_Lexing_Error,((
_tmp3D9.f1=((_tmp3D8="some action didn't return!",_tag_dyneither(_tmp3D8,sizeof(
char),27))),_tmp3D9)))),_tmp3D7)))));};}int Cyc_string(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_string_rec(
lexbuf,4);}int Cyc_slurp_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_slurp_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL4E: return 0;case 1: _LL4F: Cyc_fputc((int)'"',(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));while(Cyc_slurp_string(lexbuf)){;}
return 1;case 2: _LL50: Cyc_fputs((const char*)"*__IGNORE_FOR_CYCLONE_MALLOC(",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));{const char*_tmp3DC;void*_tmp3DB;(
_tmp3DB=0,Cyc_log(((_tmp3DC="Warning: declaration of malloc sidestepped\n",
_tag_dyneither(_tmp3DC,sizeof(char),44))),_tag_dyneither(_tmp3DB,sizeof(void*),0)));}
return 1;case 3: _LL51: Cyc_fputs((const char*)" __IGNORE_FOR_CYCLONE_MALLOC(",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));{const char*_tmp3DF;void*_tmp3DE;(
_tmp3DE=0,Cyc_log(((_tmp3DF="Warning: declaration of malloc sidestepped\n",
_tag_dyneither(_tmp3DF,sizeof(char),44))),_tag_dyneither(_tmp3DE,sizeof(void*),0)));}
return 1;case 4: _LL52: Cyc_fputs((const char*)"*__IGNORE_FOR_CYCLONE_CALLOC(",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));{const char*_tmp3E2;void*_tmp3E1;(
_tmp3E1=0,Cyc_log(((_tmp3E2="Warning: declaration of calloc sidestepped\n",
_tag_dyneither(_tmp3E2,sizeof(char),44))),_tag_dyneither(_tmp3E1,sizeof(void*),0)));}
return 1;case 5: _LL53: Cyc_fputs((const char*)" __IGNORE_FOR_CYCLONE_CALLOC(",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));{const char*_tmp3E5;void*_tmp3E4;(
_tmp3E4=0,Cyc_log(((_tmp3E5="Warning: declaration of calloc sidestepped\n",
_tag_dyneither(_tmp3E5,sizeof(char),44))),_tag_dyneither(_tmp3E4,sizeof(void*),0)));}
return 1;case 6: _LL54: Cyc_fputs((const char*)"__region",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));{const char*_tmp3E8;void*_tmp3E7;(_tmp3E7=0,Cyc_log(((
_tmp3E8="Warning: use of region sidestepped\n",_tag_dyneither(_tmp3E8,sizeof(
char),36))),_tag_dyneither(_tmp3E7,sizeof(void*),0)));}return 1;case 7: _LL55:
return 1;case 8: _LL56: return 1;case 9: _LL57: return 1;case 10: _LL58: return 1;case 11:
_LL59: return 1;case 12: _LL5A: return 1;case 13: _LL5B: Cyc_fputs((const char*)"inline",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 14: _LL5C: Cyc_fputs((
const char*)"inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;
case 15: _LL5D: Cyc_fputs((const char*)"const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
return 1;case 16: _LL5E: Cyc_fputs((const char*)"const",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));return 1;case 17: _LL5F: Cyc_fputs((const char*)"int",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 18: _LL60: return 1;
case 19: _LL61: Cyc_parens_to_match=1;while(Cyc_asm(lexbuf)){;}Cyc_fputs((const char*)"0",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));{const char*_tmp3EB;void*_tmp3EA;(
_tmp3EA=0,Cyc_log(((_tmp3EB="Warning: replacing use of __asm__ with 0\n",
_tag_dyneither(_tmp3EB,sizeof(char),42))),_tag_dyneither(_tmp3EA,sizeof(void*),0)));}
return 1;case 20: _LL62: Cyc_fputc((int)Cyc_Lexing_lexeme_char(lexbuf,0),(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));return 1;default: _LL63:(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp3F1;
const char*_tmp3F0;struct Cyc_Lexing_Error_struct*_tmp3EF;(int)_throw((void*)((
_tmp3EF=_cycalloc(sizeof(*_tmp3EF)),((_tmp3EF[0]=((_tmp3F1.tag=Cyc_Lexing_Error,((
_tmp3F1.f1=((_tmp3F0="some action didn't return!",_tag_dyneither(_tmp3F0,sizeof(
char),27))),_tmp3F1)))),_tmp3EF)))));};}int Cyc_slurp(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_slurp(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(lexbuf,
5);}int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_slurp_string_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL65: return 0;case 1: _LL66: Cyc_fputc((int)'"',(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 0;case 2: _LL67:{const char*
_tmp3F4;void*_tmp3F3;(_tmp3F3=0,Cyc_log(((_tmp3F4="Warning: unclosed string\n",
_tag_dyneither(_tmp3F4,sizeof(char),26))),_tag_dyneither(_tmp3F3,sizeof(void*),0)));}{
const char*_tmp3F8;void*_tmp3F7[1];struct Cyc_String_pa_struct _tmp3F6;(_tmp3F6.tag=
0,((_tmp3F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf)),((_tmp3F7[0]=& _tmp3F6,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((
_tmp3F8="%s",_tag_dyneither(_tmp3F8,sizeof(char),3))),_tag_dyneither(_tmp3F7,
sizeof(void*),1)))))));}return 1;case 3: _LL68:{const char*_tmp3FC;void*_tmp3FB[1];
struct Cyc_String_pa_struct _tmp3FA;(_tmp3FA.tag=0,((_tmp3FA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp3FB[0]=&
_tmp3FA,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp3FC="%s",
_tag_dyneither(_tmp3FC,sizeof(char),3))),_tag_dyneither(_tmp3FB,sizeof(void*),1)))))));}
return 1;case 4: _LL69:{const char*_tmp400;void*_tmp3FF[1];struct Cyc_String_pa_struct
_tmp3FE;(_tmp3FE.tag=0,((_tmp3FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Lexing_lexeme(lexbuf)),((_tmp3FF[0]=& _tmp3FE,Cyc_fprintf((struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out),((_tmp400="%s",_tag_dyneither(_tmp400,sizeof(char),3))),
_tag_dyneither(_tmp3FF,sizeof(void*),1)))))));}return 1;case 5: _LL6A:{const char*
_tmp404;void*_tmp403[1];struct Cyc_String_pa_struct _tmp402;(_tmp402.tag=0,((
_tmp402.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((
_tmp403[0]=& _tmp402,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((
_tmp404="%s",_tag_dyneither(_tmp404,sizeof(char),3))),_tag_dyneither(_tmp403,
sizeof(void*),1)))))));}return 1;case 6: _LL6B:{const char*_tmp408;void*_tmp407[1];
struct Cyc_String_pa_struct _tmp406;(_tmp406.tag=0,((_tmp406.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp407[0]=&
_tmp406,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp408="%s",
_tag_dyneither(_tmp408,sizeof(char),3))),_tag_dyneither(_tmp407,sizeof(void*),1)))))));}
return 1;case 7: _LL6C:{const char*_tmp40C;void*_tmp40B[1];struct Cyc_String_pa_struct
_tmp40A;(_tmp40A.tag=0,((_tmp40A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Lexing_lexeme(lexbuf)),((_tmp40B[0]=& _tmp40A,Cyc_fprintf((struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out),((_tmp40C="%s",_tag_dyneither(_tmp40C,sizeof(char),3))),
_tag_dyneither(_tmp40B,sizeof(void*),1)))))));}return 1;case 8: _LL6D:{const char*
_tmp410;void*_tmp40F[1];struct Cyc_String_pa_struct _tmp40E;(_tmp40E.tag=0,((
_tmp40E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((
_tmp40F[0]=& _tmp40E,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((
_tmp410="%s",_tag_dyneither(_tmp410,sizeof(char),3))),_tag_dyneither(_tmp40F,
sizeof(void*),1)))))));}return 1;default: _LL6E:(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_string_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp416;const char*_tmp415;struct Cyc_Lexing_Error_struct*_tmp414;(int)_throw((
void*)((_tmp414=_cycalloc(sizeof(*_tmp414)),((_tmp414[0]=((_tmp416.tag=Cyc_Lexing_Error,((
_tmp416.f1=((_tmp415="some action didn't return!",_tag_dyneither(_tmp415,sizeof(
char),27))),_tmp416)))),_tmp414)))));};}int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_string_rec(
lexbuf,6);}int Cyc_asm_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_asm_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL70: return 0;case 1: _LL71: if(Cyc_parens_to_match
== 1)return 0;-- Cyc_parens_to_match;return 1;case 2: _LL72: ++ Cyc_parens_to_match;
return 1;case 3: _LL73: while(Cyc_asm_string(lexbuf)){;}return 1;case 4: _LL74: while(
Cyc_asm_comment(lexbuf)){;}return 1;case 5: _LL75: return 1;case 6: _LL76: return 1;
default: _LL77:(lexbuf->refill_buff)(lexbuf);return Cyc_asm_rec(lexbuf,lexstate);}{
struct Cyc_Lexing_Error_struct _tmp41C;const char*_tmp41B;struct Cyc_Lexing_Error_struct*
_tmp41A;(int)_throw((void*)((_tmp41A=_cycalloc(sizeof(*_tmp41A)),((_tmp41A[0]=((
_tmp41C.tag=Cyc_Lexing_Error,((_tmp41C.f1=((_tmp41B="some action didn't return!",
_tag_dyneither(_tmp41B,sizeof(char),27))),_tmp41C)))),_tmp41A)))));};}int Cyc_asm(
struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_asm(struct Cyc_Lexing_lexbuf*lexbuf){
return Cyc_asm_rec(lexbuf,7);}int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate);int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL79:{
const char*_tmp41F;void*_tmp41E;(_tmp41E=0,Cyc_log(((_tmp41F="Warning: unclosed string\n",
_tag_dyneither(_tmp41F,sizeof(char),26))),_tag_dyneither(_tmp41E,sizeof(void*),0)));}
return 0;case 1: _LL7A: return 0;case 2: _LL7B:{const char*_tmp422;void*_tmp421;(_tmp421=
0,Cyc_log(((_tmp422="Warning: unclosed string\n",_tag_dyneither(_tmp422,sizeof(
char),26))),_tag_dyneither(_tmp421,sizeof(void*),0)));}return 1;case 3: _LL7C:
return 1;case 4: _LL7D: return 1;case 5: _LL7E: return 1;case 6: _LL7F: return 1;case 7: _LL80:
return 1;case 8: _LL81: return 1;default: _LL82:(lexbuf->refill_buff)(lexbuf);return
Cyc_asm_string_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp428;const
char*_tmp427;struct Cyc_Lexing_Error_struct*_tmp426;(int)_throw((void*)((_tmp426=
_cycalloc(sizeof(*_tmp426)),((_tmp426[0]=((_tmp428.tag=Cyc_Lexing_Error,((
_tmp428.f1=((_tmp427="some action didn't return!",_tag_dyneither(_tmp427,sizeof(
char),27))),_tmp428)))),_tmp426)))));};}int Cyc_asm_string(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_asm_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_string_rec(
lexbuf,8);}int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);
int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(
lexstate,lexbuf);switch(lexstate){case 0: _LL84:{const char*_tmp42B;void*_tmp42A;(
_tmp42A=0,Cyc_log(((_tmp42B="Warning: unclosed comment\n",_tag_dyneither(_tmp42B,
sizeof(char),27))),_tag_dyneither(_tmp42A,sizeof(void*),0)));}return 0;case 1:
_LL85: return 0;case 2: _LL86: return 1;default: _LL87:(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_comment_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp431;const char*_tmp430;struct Cyc_Lexing_Error_struct*_tmp42F;(int)_throw((
void*)((_tmp42F=_cycalloc(sizeof(*_tmp42F)),((_tmp42F[0]=((_tmp431.tag=Cyc_Lexing_Error,((
_tmp431.f1=((_tmp430="some action didn't return!",_tag_dyneither(_tmp430,sizeof(
char),27))),_tmp431)))),_tmp42F)))));};}int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_comment_rec(
lexbuf,9);}struct _tuple18*Cyc_suck_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate);struct _tuple18*Cyc_suck_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL89:{
const char*_tmp432;Cyc_current_line=((_tmp432="#define ",_tag_dyneither(_tmp432,
sizeof(char),9)));}Cyc_suck_macroname(lexbuf);{struct _tuple18*_tmp433;return(
_tmp433=_cycalloc(sizeof(*_tmp433)),((_tmp433->f1=Cyc_current_line,((_tmp433->f2=(
struct _dyneither_ptr*)_check_null(Cyc_current_source),_tmp433)))));};case 1: _LL8A:
return Cyc_suck_line(lexbuf);case 2: _LL8B: return 0;default: _LL8C:(lexbuf->refill_buff)(
lexbuf);return Cyc_suck_line_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp439;const char*_tmp438;struct Cyc_Lexing_Error_struct*_tmp437;(int)_throw((
void*)((_tmp437=_cycalloc(sizeof(*_tmp437)),((_tmp437[0]=((_tmp439.tag=Cyc_Lexing_Error,((
_tmp439.f1=((_tmp438="some action didn't return!",_tag_dyneither(_tmp438,sizeof(
char),27))),_tmp439)))),_tmp437)))));};}struct _tuple18*Cyc_suck_line(struct Cyc_Lexing_lexbuf*
lexbuf);struct _tuple18*Cyc_suck_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(
lexbuf,10);}int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);
int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=
Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL8E:{struct
_dyneither_ptr*_tmp43A;Cyc_current_source=((_tmp43A=_cycalloc(sizeof(*_tmp43A)),((
_tmp43A[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),_tmp43A))));}Cyc_current_line=(
struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_current_line,(
struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_current_source)));
return Cyc_suck_restofline(lexbuf);default: _LL8F:(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_macroname_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp440;const char*_tmp43F;struct Cyc_Lexing_Error_struct*_tmp43E;(int)_throw((
void*)((_tmp43E=_cycalloc(sizeof(*_tmp43E)),((_tmp43E[0]=((_tmp440.tag=Cyc_Lexing_Error,((
_tmp440.f1=((_tmp43F="some action didn't return!",_tag_dyneither(_tmp43F,sizeof(
char),27))),_tmp440)))),_tmp43E)))));};}int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_macroname_rec(
lexbuf,11);}int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate);int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL91: Cyc_current_line=(
struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_current_line,(
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 0;default: _LL92:(lexbuf->refill_buff)(
lexbuf);return Cyc_suck_restofline_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp446;const char*_tmp445;struct Cyc_Lexing_Error_struct*_tmp444;(int)_throw((
void*)((_tmp444=_cycalloc(sizeof(*_tmp444)),((_tmp444[0]=((_tmp446.tag=Cyc_Lexing_Error,((
_tmp446.f1=((_tmp445="some action didn't return!",_tag_dyneither(_tmp445,sizeof(
char),27))),_tmp446)))),_tmp444)))));};}int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(
lexbuf,12);}struct _tuple20*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate);struct _tuple20*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL94: return Cyc_spec(
lexbuf);case 1: _LL95: Cyc_current_headerfile=(struct _dyneither_ptr)Cyc_substring((
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1));Cyc_current_symbols=0;Cyc_current_omit_symbols=
0;Cyc_current_cstubs=0;Cyc_current_cycstubs=0;Cyc_current_hstubs=0;while(Cyc_commands(
lexbuf)){;}Cyc_current_hstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_current_hstubs);Cyc_current_cstubs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cstubs);Cyc_current_cycstubs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cycstubs);{
struct _tuple20*_tmp447;return(_tmp447=_cycalloc(sizeof(*_tmp447)),((_tmp447->f1=
Cyc_current_headerfile,((_tmp447->f2=Cyc_current_symbols,((_tmp447->f3=Cyc_current_omit_symbols,((
_tmp447->f4=Cyc_current_hstubs,((_tmp447->f5=Cyc_current_cstubs,((_tmp447->f6=
Cyc_current_cycstubs,_tmp447)))))))))))));};case 2: _LL96: return Cyc_spec(lexbuf);
case 3: _LL97: return 0;case 4: _LL98:{const char*_tmp44B;void*_tmp44A[1];struct Cyc_Int_pa_struct
_tmp449;(_tmp449.tag=1,((_tmp449.f1=(unsigned long)((int)Cyc_Lexing_lexeme_char(
lexbuf,0)),((_tmp44A[0]=& _tmp449,Cyc_fprintf(Cyc_stderr,((_tmp44B="Error in .cys file: expected header file name, found '%c' instead\n",
_tag_dyneither(_tmp44B,sizeof(char),67))),_tag_dyneither(_tmp44A,sizeof(void*),1)))))));}
return 0;default: _LL99:(lexbuf->refill_buff)(lexbuf);return Cyc_spec_rec(lexbuf,
lexstate);}{struct Cyc_Lexing_Error_struct _tmp451;const char*_tmp450;struct Cyc_Lexing_Error_struct*
_tmp44F;(int)_throw((void*)((_tmp44F=_cycalloc(sizeof(*_tmp44F)),((_tmp44F[0]=((
_tmp451.tag=Cyc_Lexing_Error,((_tmp451.f1=((_tmp450="some action didn't return!",
_tag_dyneither(_tmp450,sizeof(char),27))),_tmp451)))),_tmp44F)))));};}struct
_tuple20*Cyc_spec(struct Cyc_Lexing_lexbuf*lexbuf);struct _tuple20*Cyc_spec(struct
Cyc_Lexing_lexbuf*lexbuf){return Cyc_spec_rec(lexbuf,13);}int Cyc_commands_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL9B: return 0;case 1: _LL9C: return 0;case 2: _LL9D: Cyc_snarfed_symbols=0;while(
Cyc_snarfsymbols(lexbuf)){;}Cyc_current_symbols=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_symbols);
return 1;case 3: _LL9E: Cyc_snarfed_symbols=0;while(Cyc_snarfsymbols(lexbuf)){;}Cyc_current_omit_symbols=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_snarfed_symbols,Cyc_current_omit_symbols);return 1;case 4: _LL9F: Cyc_braces_to_match=
1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{
struct _tuple19*_tmp452;struct _tuple19*x=(_tmp452=_cycalloc(sizeof(*_tmp452)),((
_tmp452->f1=(struct _dyneither_ptr)_tag_dyneither(0,0,0),((_tmp452->f2=(struct
_dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),
_tmp452)))));{struct Cyc_List_List*_tmp453;Cyc_current_hstubs=((_tmp453=_cycalloc(
sizeof(*_tmp453)),((_tmp453->hd=x,((_tmp453->tl=Cyc_current_hstubs,_tmp453))))));}
return 1;};case 5: _LLA0: {struct _dyneither_ptr _tmp75=Cyc_Lexing_lexeme(lexbuf);{
const char*_tmp454;_dyneither_ptr_inplace_plus(& _tmp75,sizeof(char),(int)Cyc_strlen(((
_tmp454="hstub",_tag_dyneither(_tmp454,sizeof(char),6)))));}while(isspace((int)*((
char*)_check_dyneither_subscript(_tmp75,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& _tmp75,sizeof(char),1);}{struct _dyneither_ptr t=
_tmp75;while(!isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{struct _dyneither_ptr _tmp77=Cyc_substring((
struct _dyneither_ptr)_tmp75,0,(unsigned long)((t.curr - _tmp75.curr)/ sizeof(char)));
Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{struct _tuple19*_tmp455;struct _tuple19*x=(_tmp455=
_cycalloc(sizeof(*_tmp455)),((_tmp455->f1=(struct _dyneither_ptr)_tmp77,((_tmp455->f2=(
struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),
_tmp455)))));{struct Cyc_List_List*_tmp456;Cyc_current_hstubs=((_tmp456=_cycalloc(
sizeof(*_tmp456)),((_tmp456->hd=x,((_tmp456->tl=Cyc_current_hstubs,_tmp456))))));}
return 1;};};};}case 6: _LLA1: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)
Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple19*_tmp457;struct
_tuple19*x=(_tmp457=_cycalloc(sizeof(*_tmp457)),((_tmp457->f1=(struct
_dyneither_ptr)_tag_dyneither(0,0,0),((_tmp457->f2=(struct _dyneither_ptr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp457)))));{struct Cyc_List_List*
_tmp458;Cyc_current_cstubs=((_tmp458=_cycalloc(sizeof(*_tmp458)),((_tmp458->hd=x,((
_tmp458->tl=Cyc_current_cstubs,_tmp458))))));}return 1;};case 7: _LLA2: {struct
_dyneither_ptr _tmp7C=Cyc_Lexing_lexeme(lexbuf);{const char*_tmp459;
_dyneither_ptr_inplace_plus(& _tmp7C,sizeof(char),(int)Cyc_strlen(((_tmp459="cstub",
_tag_dyneither(_tmp459,sizeof(char),6)))));}while(isspace((int)*((char*)
_check_dyneither_subscript(_tmp7C,sizeof(char),0)))){_dyneither_ptr_inplace_plus(&
_tmp7C,sizeof(char),1);}{struct _dyneither_ptr t=_tmp7C;while(!isspace((int)*((
char*)_check_dyneither_subscript(t,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{struct _dyneither_ptr _tmp7E=Cyc_substring((
struct _dyneither_ptr)_tmp7C,0,(unsigned long)((t.curr - _tmp7C.curr)/ sizeof(char)));
Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{struct _tuple19*_tmp45A;struct _tuple19*x=(_tmp45A=
_cycalloc(sizeof(*_tmp45A)),((_tmp45A->f1=(struct _dyneither_ptr)_tmp7E,((_tmp45A->f2=(
struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),
_tmp45A)))));{struct Cyc_List_List*_tmp45B;Cyc_current_cstubs=((_tmp45B=_cycalloc(
sizeof(*_tmp45B)),((_tmp45B->hd=x,((_tmp45B->tl=Cyc_current_cstubs,_tmp45B))))));}
return 1;};};};}case 8: _LLA3: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)
Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple19*_tmp45C;struct
_tuple19*x=(_tmp45C=_cycalloc(sizeof(*_tmp45C)),((_tmp45C->f1=(struct
_dyneither_ptr)_tag_dyneither(0,0,0),((_tmp45C->f2=(struct _dyneither_ptr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp45C)))));{struct Cyc_List_List*
_tmp45D;Cyc_current_cycstubs=((_tmp45D=_cycalloc(sizeof(*_tmp45D)),((_tmp45D->hd=
x,((_tmp45D->tl=Cyc_current_cycstubs,_tmp45D))))));}return 1;};case 9: _LLA4: {
struct _dyneither_ptr _tmp83=Cyc_Lexing_lexeme(lexbuf);{const char*_tmp45E;
_dyneither_ptr_inplace_plus(& _tmp83,sizeof(char),(int)Cyc_strlen(((_tmp45E="cycstub",
_tag_dyneither(_tmp45E,sizeof(char),8)))));}while(isspace((int)*((char*)
_check_dyneither_subscript(_tmp83,sizeof(char),0)))){_dyneither_ptr_inplace_plus(&
_tmp83,sizeof(char),1);}{struct _dyneither_ptr t=_tmp83;while(!isspace((int)*((
char*)_check_dyneither_subscript(t,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{struct _dyneither_ptr _tmp85=Cyc_substring((
struct _dyneither_ptr)_tmp83,0,(unsigned long)((t.curr - _tmp83.curr)/ sizeof(char)));
Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{struct _tuple19*_tmp45F;struct _tuple19*x=(_tmp45F=
_cycalloc(sizeof(*_tmp45F)),((_tmp45F->f1=(struct _dyneither_ptr)_tmp85,((_tmp45F->f2=(
struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),
_tmp45F)))));{struct Cyc_List_List*_tmp460;Cyc_current_cycstubs=((_tmp460=
_cycalloc(sizeof(*_tmp460)),((_tmp460->hd=x,((_tmp460->tl=Cyc_current_cycstubs,
_tmp460))))));}return 1;};};};}case 10: _LLA5: Cyc_braces_to_match=1;Cyc_specbuf=(
struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct
_dyneither_ptr*_tmp461;struct _dyneither_ptr*x=(_tmp461=_cycalloc(sizeof(*_tmp461)),((
_tmp461[0]=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf)),_tmp461)));{struct Cyc_List_List*_tmp462;Cyc_current_cpp=((
_tmp462=_cycalloc(sizeof(*_tmp462)),((_tmp462->hd=x,((_tmp462->tl=Cyc_current_cpp,
_tmp462))))));}return 1;};case 11: _LLA6: return 1;case 12: _LLA7: return 1;case 13: _LLA8:{
const char*_tmp466;void*_tmp465[1];struct Cyc_Int_pa_struct _tmp464;(_tmp464.tag=1,((
_tmp464.f1=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0)),((_tmp465[0]=&
_tmp464,Cyc_fprintf(Cyc_stderr,((_tmp466="Error in .cys file: expected command, found '%c' instead\n",
_tag_dyneither(_tmp466,sizeof(char),58))),_tag_dyneither(_tmp465,sizeof(void*),1)))))));}
return 0;default: _LLA9:(lexbuf->refill_buff)(lexbuf);return Cyc_commands_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp46C;const char*_tmp46B;struct
Cyc_Lexing_Error_struct*_tmp46A;(int)_throw((void*)((_tmp46A=_cycalloc(sizeof(*
_tmp46A)),((_tmp46A[0]=((_tmp46C.tag=Cyc_Lexing_Error,((_tmp46C.f1=((_tmp46B="some action didn't return!",
_tag_dyneither(_tmp46B,sizeof(char),27))),_tmp46C)))),_tmp46A)))));};}int Cyc_commands(
struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf){
return Cyc_commands_rec(lexbuf,14);}int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate);int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLAB:{
struct _dyneither_ptr*_tmp46F;struct Cyc_List_List*_tmp46E;Cyc_snarfed_symbols=((
_tmp46E=_cycalloc(sizeof(*_tmp46E)),((_tmp46E->hd=((_tmp46F=_cycalloc(sizeof(*
_tmp46F)),((_tmp46F[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),_tmp46F)))),((
_tmp46E->tl=Cyc_snarfed_symbols,_tmp46E))))));}return 1;case 1: _LLAC: return 1;case 2:
_LLAD: return 0;case 3: _LLAE:{const char*_tmp472;void*_tmp471;(_tmp471=0,Cyc_fprintf(
Cyc_stderr,((_tmp472="Error in .cys file: unexpected end-of-file\n",
_tag_dyneither(_tmp472,sizeof(char),44))),_tag_dyneither(_tmp471,sizeof(void*),0)));}
return 0;case 4: _LLAF:{const char*_tmp476;void*_tmp475[1];struct Cyc_Int_pa_struct
_tmp474;(_tmp474.tag=1,((_tmp474.f1=(unsigned long)((int)Cyc_Lexing_lexeme_char(
lexbuf,0)),((_tmp475[0]=& _tmp474,Cyc_fprintf(Cyc_stderr,((_tmp476="Error in .cys file: expected symbol, found '%c' instead\n",
_tag_dyneither(_tmp476,sizeof(char),57))),_tag_dyneither(_tmp475,sizeof(void*),1)))))));}
return 0;default: _LLB0:(lexbuf->refill_buff)(lexbuf);return Cyc_snarfsymbols_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp47C;const char*_tmp47B;struct
Cyc_Lexing_Error_struct*_tmp47A;(int)_throw((void*)((_tmp47A=_cycalloc(sizeof(*
_tmp47A)),((_tmp47A[0]=((_tmp47C.tag=Cyc_Lexing_Error,((_tmp47C.f1=((_tmp47B="some action didn't return!",
_tag_dyneither(_tmp47B,sizeof(char),27))),_tmp47C)))),_tmp47A)))));};}int Cyc_snarfsymbols(
struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_snarfsymbols_rec(lexbuf,15);}int Cyc_block_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate);int Cyc_block_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLB2:{const char*
_tmp47F;void*_tmp47E;(_tmp47E=0,Cyc_log(((_tmp47F="Warning: unclosed brace\n",
_tag_dyneither(_tmp47F,sizeof(char),25))),_tag_dyneither(_tmp47E,sizeof(void*),0)));}
return 0;case 1: _LLB3: if(Cyc_braces_to_match == 1)return 0;-- Cyc_braces_to_match;Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'}');return 1;case 2: _LLB4: ++ Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'{');return 1;
case 3: _LLB5: Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');
while(Cyc_block_string(lexbuf)){;}return 1;case 4: _LLB6:{const char*_tmp480;Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),((_tmp480="/*",_tag_dyneither(
_tmp480,sizeof(char),3))));}while(Cyc_block_comment(lexbuf)){;}return 1;case 5:
_LLB7: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 6: _LLB8: Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),Cyc_Lexing_lexeme_char(lexbuf,0));
return 1;default: _LLB9:(lexbuf->refill_buff)(lexbuf);return Cyc_block_rec(lexbuf,
lexstate);}{struct Cyc_Lexing_Error_struct _tmp486;const char*_tmp485;struct Cyc_Lexing_Error_struct*
_tmp484;(int)_throw((void*)((_tmp484=_cycalloc(sizeof(*_tmp484)),((_tmp484[0]=((
_tmp486.tag=Cyc_Lexing_Error,((_tmp486.f1=((_tmp485="some action didn't return!",
_tag_dyneither(_tmp485,sizeof(char),27))),_tmp486)))),_tmp484)))));};}int Cyc_block(
struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_block(struct Cyc_Lexing_lexbuf*lexbuf){
return Cyc_block_rec(lexbuf,16);}int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate);int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLBB:{
const char*_tmp489;void*_tmp488;(_tmp488=0,Cyc_log(((_tmp489="Warning: unclosed string\n",
_tag_dyneither(_tmp489,sizeof(char),26))),_tag_dyneither(_tmp488,sizeof(void*),0)));}
return 0;case 1: _LLBC: Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');
return 0;case 2: _LLBD:{const char*_tmp48C;void*_tmp48B;(_tmp48B=0,Cyc_log(((_tmp48C="Warning: unclosed string\n",
_tag_dyneither(_tmp48C,sizeof(char),26))),_tag_dyneither(_tmp48B,sizeof(void*),0)));}
Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct
_dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 3: _LLBE: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 4: _LLBF: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return
1;case 5: _LLC0: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 6: _LLC1: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 7: _LLC2: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return
1;case 8: _LLC3: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;default: _LLC4:(lexbuf->refill_buff)(
lexbuf);return Cyc_block_string_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp492;const char*_tmp491;struct Cyc_Lexing_Error_struct*_tmp490;(int)_throw((
void*)((_tmp490=_cycalloc(sizeof(*_tmp490)),((_tmp490[0]=((_tmp492.tag=Cyc_Lexing_Error,((
_tmp492.f1=((_tmp491="some action didn't return!",_tag_dyneither(_tmp491,sizeof(
char),27))),_tmp492)))),_tmp490)))));};}int Cyc_block_string(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_string_rec(
lexbuf,17);}int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);
int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=
Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLC6:{const char*_tmp495;
void*_tmp494;(_tmp494=0,Cyc_log(((_tmp495="Warning: unclosed comment\n",
_tag_dyneither(_tmp495,sizeof(char),27))),_tag_dyneither(_tmp494,sizeof(void*),0)));}
return 0;case 1: _LLC7:{const char*_tmp496;Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),((_tmp496="*/",_tag_dyneither(_tmp496,sizeof(char),3))));}
return 0;case 2: _LLC8: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;default: _LLC9:(lexbuf->refill_buff)(
lexbuf);return Cyc_block_comment_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp49C;const char*_tmp49B;struct Cyc_Lexing_Error_struct*_tmp49A;(int)_throw((
void*)((_tmp49A=_cycalloc(sizeof(*_tmp49A)),((_tmp49A[0]=((_tmp49C.tag=Cyc_Lexing_Error,((
_tmp49C.f1=((_tmp49B="some action didn't return!",_tag_dyneither(_tmp49B,sizeof(
char),27))),_tmp49C)))),_tmp49A)))));};}int Cyc_block_comment(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_comment_rec(
lexbuf,18);}void Cyc_scan_type(void*t);struct _tuple21{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};void Cyc_scan_exp(struct Cyc_Absyn_Exp*e);void Cyc_scan_exp(
struct Cyc_Absyn_Exp*e){void*_tmpAD=((struct Cyc_Absyn_Exp*)_check_null(e))->r;
struct _tuple0*_tmpAF;struct _tuple0 _tmpB0;struct _dyneither_ptr*_tmpB1;struct
_tuple0*_tmpB3;struct _tuple0 _tmpB4;struct _dyneither_ptr*_tmpB5;struct Cyc_List_List*
_tmpB7;struct Cyc_Absyn_Exp*_tmpB9;struct Cyc_Absyn_Exp*_tmpBA;struct Cyc_Absyn_Exp*
_tmpBC;struct Cyc_Absyn_Exp*_tmpBD;struct Cyc_Absyn_Exp*_tmpBF;struct Cyc_Absyn_Exp*
_tmpC0;struct Cyc_Absyn_Exp*_tmpC2;struct Cyc_Absyn_Exp*_tmpC4;struct Cyc_Absyn_Exp*
_tmpC6;struct Cyc_Absyn_Exp*_tmpC8;struct Cyc_Absyn_Exp*_tmpCA;struct Cyc_Absyn_Exp*
_tmpCB;struct Cyc_Absyn_Exp*_tmpCC;struct Cyc_Absyn_Exp*_tmpCE;struct Cyc_Absyn_Exp*
_tmpCF;struct Cyc_Absyn_Exp*_tmpD1;struct Cyc_Absyn_Exp*_tmpD2;struct Cyc_Absyn_Exp*
_tmpD4;struct Cyc_List_List*_tmpD5;struct Cyc_Absyn_Exp*_tmpD7;struct Cyc_List_List*
_tmpD8;void*_tmpDA;struct Cyc_Absyn_Exp*_tmpDB;struct Cyc_Absyn_MallocInfo _tmpDD;
int _tmpDE;struct Cyc_Absyn_Exp*_tmpDF;void**_tmpE0;struct Cyc_Absyn_Exp*_tmpE1;
struct Cyc_Absyn_Exp*_tmpE3;void*_tmpE5;void*_tmpE7;struct Cyc_Absyn_Exp*_tmpE9;
struct _dyneither_ptr*_tmpEA;struct Cyc_Absyn_Exp*_tmpEC;struct _dyneither_ptr*
_tmpED;void*_tmpEF;void*_tmpF0;struct Cyc_List_List*_tmpF3;_LLCC: {struct Cyc_Absyn_Var_e_struct*
_tmpAE=(struct Cyc_Absyn_Var_e_struct*)_tmpAD;if(_tmpAE->tag != 1)goto _LLCE;else{
_tmpAF=_tmpAE->f1;_tmpB0=*_tmpAF;_tmpB1=_tmpB0.f2;}}_LLCD: _tmpB5=_tmpB1;goto
_LLCF;_LLCE: {struct Cyc_Absyn_UnknownId_e_struct*_tmpB2=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmpAD;if(_tmpB2->tag != 2)goto _LLD0;else{_tmpB3=_tmpB2->f1;_tmpB4=*_tmpB3;_tmpB5=
_tmpB4.f2;}}_LLCF: Cyc_add_target(_tmpB5);return;_LLD0: {struct Cyc_Absyn_Primop_e_struct*
_tmpB6=(struct Cyc_Absyn_Primop_e_struct*)_tmpAD;if(_tmpB6->tag != 3)goto _LLD2;
else{_tmpB7=_tmpB6->f2;}}_LLD1: for(0;_tmpB7 != 0;_tmpB7=_tmpB7->tl){Cyc_scan_exp((
struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Exp*)_tmpB7->hd));}return;_LLD2: {struct
Cyc_Absyn_Subscript_e_struct*_tmpB8=(struct Cyc_Absyn_Subscript_e_struct*)_tmpAD;
if(_tmpB8->tag != 25)goto _LLD4;else{_tmpB9=_tmpB8->f1;_tmpBA=_tmpB8->f2;}}_LLD3:
_tmpBC=_tmpB9;_tmpBD=_tmpBA;goto _LLD5;_LLD4: {struct Cyc_Absyn_SeqExp_e_struct*
_tmpBB=(struct Cyc_Absyn_SeqExp_e_struct*)_tmpAD;if(_tmpBB->tag != 9)goto _LLD6;
else{_tmpBC=_tmpBB->f1;_tmpBD=_tmpBB->f2;}}_LLD5: _tmpBF=_tmpBC;_tmpC0=_tmpBD;
goto _LLD7;_LLD6: {struct Cyc_Absyn_AssignOp_e_struct*_tmpBE=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmpAD;if(_tmpBE->tag != 4)goto _LLD8;else{_tmpBF=_tmpBE->f1;_tmpC0=_tmpBE->f3;}}
_LLD7: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpBF);Cyc_scan_exp((struct Cyc_Absyn_Exp*)
_tmpC0);return;_LLD8: {struct Cyc_Absyn_Deref_e_struct*_tmpC1=(struct Cyc_Absyn_Deref_e_struct*)
_tmpAD;if(_tmpC1->tag != 22)goto _LLDA;else{_tmpC2=_tmpC1->f1;}}_LLD9: _tmpC4=
_tmpC2;goto _LLDB;_LLDA: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmpC3=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmpAD;if(_tmpC3->tag != 19)goto _LLDC;else{_tmpC4=_tmpC3->f1;}}_LLDB: _tmpC6=
_tmpC4;goto _LLDD;_LLDC: {struct Cyc_Absyn_Address_e_struct*_tmpC5=(struct Cyc_Absyn_Address_e_struct*)
_tmpAD;if(_tmpC5->tag != 16)goto _LLDE;else{_tmpC6=_tmpC5->f1;}}_LLDD: _tmpC8=
_tmpC6;goto _LLDF;_LLDE: {struct Cyc_Absyn_Increment_e_struct*_tmpC7=(struct Cyc_Absyn_Increment_e_struct*)
_tmpAD;if(_tmpC7->tag != 5)goto _LLE0;else{_tmpC8=_tmpC7->f1;}}_LLDF: Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpC8);return;_LLE0: {struct Cyc_Absyn_Conditional_e_struct*
_tmpC9=(struct Cyc_Absyn_Conditional_e_struct*)_tmpAD;if(_tmpC9->tag != 6)goto
_LLE2;else{_tmpCA=_tmpC9->f1;_tmpCB=_tmpC9->f2;_tmpCC=_tmpC9->f3;}}_LLE1: Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpCA);Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpCB);Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpCC);return;_LLE2: {struct Cyc_Absyn_And_e_struct*_tmpCD=(
struct Cyc_Absyn_And_e_struct*)_tmpAD;if(_tmpCD->tag != 7)goto _LLE4;else{_tmpCE=
_tmpCD->f1;_tmpCF=_tmpCD->f2;}}_LLE3: _tmpD1=_tmpCE;_tmpD2=_tmpCF;goto _LLE5;_LLE4: {
struct Cyc_Absyn_Or_e_struct*_tmpD0=(struct Cyc_Absyn_Or_e_struct*)_tmpAD;if(
_tmpD0->tag != 8)goto _LLE6;else{_tmpD1=_tmpD0->f1;_tmpD2=_tmpD0->f2;}}_LLE5: Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpD1);Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpD2);return;
_LLE6: {struct Cyc_Absyn_FnCall_e_struct*_tmpD3=(struct Cyc_Absyn_FnCall_e_struct*)
_tmpAD;if(_tmpD3->tag != 11)goto _LLE8;else{_tmpD4=_tmpD3->f1;_tmpD5=_tmpD3->f2;}}
_LLE7: _tmpD7=_tmpD4;_tmpD8=_tmpD5;goto _LLE9;_LLE8: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmpD6=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmpAD;if(_tmpD6->tag != 10)goto
_LLEA;else{_tmpD7=_tmpD6->f1;_tmpD8=_tmpD6->f2;}}_LLE9: Cyc_scan_exp((struct Cyc_Absyn_Exp*)
_tmpD7);for(0;_tmpD8 != 0;_tmpD8=_tmpD8->tl){Cyc_scan_exp((struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*)_tmpD8->hd));}return;_LLEA: {struct Cyc_Absyn_Cast_e_struct*
_tmpD9=(struct Cyc_Absyn_Cast_e_struct*)_tmpAD;if(_tmpD9->tag != 15)goto _LLEC;
else{_tmpDA=(void*)_tmpD9->f1;_tmpDB=_tmpD9->f2;}}_LLEB: Cyc_scan_type(_tmpDA);
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpDB);return;_LLEC: {struct Cyc_Absyn_Malloc_e_struct*
_tmpDC=(struct Cyc_Absyn_Malloc_e_struct*)_tmpAD;if(_tmpDC->tag != 35)goto _LLEE;
else{_tmpDD=_tmpDC->f1;_tmpDE=_tmpDD.is_calloc;_tmpDF=_tmpDD.rgn;_tmpE0=_tmpDD.elt_type;
_tmpE1=_tmpDD.num_elts;}}_LLED: if(_tmpDF != 0)Cyc_scan_exp((struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*)_tmpDF));if(_tmpE0 != 0)Cyc_scan_type(*_tmpE0);Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpE1);return;_LLEE: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmpE2=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmpAD;if(_tmpE2->tag != 39)goto _LLF0;
else{_tmpE3=_tmpE2->f1;}}_LLEF: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpE3);
return;_LLF0: {struct Cyc_Absyn_Valueof_e_struct*_tmpE4=(struct Cyc_Absyn_Valueof_e_struct*)
_tmpAD;if(_tmpE4->tag != 40)goto _LLF2;else{_tmpE5=(void*)_tmpE4->f1;}}_LLF1:
_tmpE7=_tmpE5;goto _LLF3;_LLF2: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpE6=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpAD;if(_tmpE6->tag != 18)goto _LLF4;else{
_tmpE7=(void*)_tmpE6->f1;}}_LLF3: Cyc_scan_type(_tmpE7);return;_LLF4: {struct Cyc_Absyn_AggrMember_e_struct*
_tmpE8=(struct Cyc_Absyn_AggrMember_e_struct*)_tmpAD;if(_tmpE8->tag != 23)goto
_LLF6;else{_tmpE9=_tmpE8->f1;_tmpEA=_tmpE8->f2;}}_LLF5: _tmpEC=_tmpE9;_tmpED=
_tmpEA;goto _LLF7;_LLF6: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpEB=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpAD;if(_tmpEB->tag != 24)goto _LLF8;else{_tmpEC=_tmpEB->f1;_tmpED=_tmpEB->f2;}}
_LLF7: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpEC);Cyc_add_target(_tmpED);return;
_LLF8: {struct Cyc_Absyn_Offsetof_e_struct*_tmpEE=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpAD;if(_tmpEE->tag != 20)goto _LLFA;else{_tmpEF=(void*)_tmpEE->f1;_tmpF0=(void*)
_tmpEE->f2;}}_LLF9: Cyc_scan_type(_tmpEF);{void*_tmp104=_tmpF0;struct
_dyneither_ptr*_tmp106;_LL11F: {struct Cyc_Absyn_StructField_struct*_tmp105=(
struct Cyc_Absyn_StructField_struct*)_tmp104;if(_tmp105->tag != 0)goto _LL121;else{
_tmp106=_tmp105->f1;}}_LL120: Cyc_add_target(_tmp106);goto _LL11E;_LL121: {struct
Cyc_Absyn_TupleIndex_struct*_tmp107=(struct Cyc_Absyn_TupleIndex_struct*)_tmp104;
if(_tmp107->tag != 1)goto _LL11E;}_LL122: goto _LL11E;_LL11E:;}return;_LLFA: {struct
Cyc_Absyn_Const_e_struct*_tmpF1=(struct Cyc_Absyn_Const_e_struct*)_tmpAD;if(
_tmpF1->tag != 0)goto _LLFC;}_LLFB: return;_LLFC: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpF2=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpAD;if(_tmpF2->tag != 37)goto
_LLFE;else{_tmpF3=_tmpF2->f2;}}_LLFD: for(0;_tmpF3 != 0;_tmpF3=_tmpF3->tl){struct
_tuple21 _tmp109;struct Cyc_Absyn_Exp*_tmp10A;struct _tuple21*_tmp108=(struct
_tuple21*)_tmpF3->hd;_tmp109=*_tmp108;_tmp10A=_tmp109.f2;Cyc_scan_exp((struct Cyc_Absyn_Exp*)
_tmp10A);}return;_LLFE: {struct Cyc_Absyn_Swap_e_struct*_tmpF4=(struct Cyc_Absyn_Swap_e_struct*)
_tmpAD;if(_tmpF4->tag != 36)goto _LL100;}_LLFF:{const char*_tmp49F;void*_tmp49E;(
_tmp49E=0,Cyc_fprintf(Cyc_stderr,((_tmp49F="Error: unexpected Swap_e\n",
_tag_dyneither(_tmp49F,sizeof(char),26))),_tag_dyneither(_tmp49E,sizeof(void*),0)));}
exit(1);return;_LL100: {struct Cyc_Absyn_StmtExp_e_struct*_tmpF5=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmpAD;if(_tmpF5->tag != 38)goto _LL102;}_LL101:{const char*_tmp4A2;void*_tmp4A1;(
_tmp4A1=0,Cyc_fprintf(Cyc_stderr,((_tmp4A2="Error: unexpected Stmt_e\n",
_tag_dyneither(_tmp4A2,sizeof(char),26))),_tag_dyneither(_tmp4A1,sizeof(void*),0)));}
exit(1);return;_LL102: {struct Cyc_Absyn_Throw_e_struct*_tmpF6=(struct Cyc_Absyn_Throw_e_struct*)
_tmpAD;if(_tmpF6->tag != 12)goto _LL104;}_LL103:{const char*_tmp4A5;void*_tmp4A4;(
_tmp4A4=0,Cyc_fprintf(Cyc_stderr,((_tmp4A5="Error: unexpected Throw_e\n",
_tag_dyneither(_tmp4A5,sizeof(char),27))),_tag_dyneither(_tmp4A4,sizeof(void*),0)));}
exit(1);return;_LL104: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmpF7=(struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmpAD;if(_tmpF7->tag != 13)goto _LL106;}_LL105:{const char*_tmp4A8;void*_tmp4A7;(
_tmp4A7=0,Cyc_fprintf(Cyc_stderr,((_tmp4A8="Error: unexpected NoInstantiate_e\n",
_tag_dyneither(_tmp4A8,sizeof(char),35))),_tag_dyneither(_tmp4A7,sizeof(void*),0)));}
exit(1);return;_LL106: {struct Cyc_Absyn_Instantiate_e_struct*_tmpF8=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmpAD;if(_tmpF8->tag != 14)goto _LL108;}_LL107:{const char*_tmp4AB;void*_tmp4AA;(
_tmp4AA=0,Cyc_fprintf(Cyc_stderr,((_tmp4AB="Error: unexpected Instantiate_e\n",
_tag_dyneither(_tmp4AB,sizeof(char),33))),_tag_dyneither(_tmp4AA,sizeof(void*),0)));}
exit(1);return;_LL108: {struct Cyc_Absyn_New_e_struct*_tmpF9=(struct Cyc_Absyn_New_e_struct*)
_tmpAD;if(_tmpF9->tag != 17)goto _LL10A;}_LL109:{const char*_tmp4AE;void*_tmp4AD;(
_tmp4AD=0,Cyc_fprintf(Cyc_stderr,((_tmp4AE="Error: unexpected New_e\n",
_tag_dyneither(_tmp4AE,sizeof(char),25))),_tag_dyneither(_tmp4AD,sizeof(void*),0)));}
exit(1);return;_LL10A: {struct Cyc_Absyn_Gentyp_e_struct*_tmpFA=(struct Cyc_Absyn_Gentyp_e_struct*)
_tmpAD;if(_tmpFA->tag != 21)goto _LL10C;}_LL10B:{const char*_tmp4B1;void*_tmp4B0;(
_tmp4B0=0,Cyc_fprintf(Cyc_stderr,((_tmp4B1="Error: unexpected Gentyp_e\n",
_tag_dyneither(_tmp4B1,sizeof(char),28))),_tag_dyneither(_tmp4B0,sizeof(void*),0)));}
exit(1);return;_LL10C: {struct Cyc_Absyn_Tuple_e_struct*_tmpFB=(struct Cyc_Absyn_Tuple_e_struct*)
_tmpAD;if(_tmpFB->tag != 26)goto _LL10E;}_LL10D:{const char*_tmp4B4;void*_tmp4B3;(
_tmp4B3=0,Cyc_fprintf(Cyc_stderr,((_tmp4B4="Error: unexpected Tuple_e\n",
_tag_dyneither(_tmp4B4,sizeof(char),27))),_tag_dyneither(_tmp4B3,sizeof(void*),0)));}
exit(1);return;_LL10E: {struct Cyc_Absyn_CompoundLit_e_struct*_tmpFC=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmpAD;if(_tmpFC->tag != 27)goto _LL110;}_LL10F:{const char*_tmp4B7;void*_tmp4B6;(
_tmp4B6=0,Cyc_fprintf(Cyc_stderr,((_tmp4B7="Error: unexpected CompoundLit_e\n",
_tag_dyneither(_tmp4B7,sizeof(char),33))),_tag_dyneither(_tmp4B6,sizeof(void*),0)));}
exit(1);return;_LL110: {struct Cyc_Absyn_Array_e_struct*_tmpFD=(struct Cyc_Absyn_Array_e_struct*)
_tmpAD;if(_tmpFD->tag != 28)goto _LL112;}_LL111:{const char*_tmp4BA;void*_tmp4B9;(
_tmp4B9=0,Cyc_fprintf(Cyc_stderr,((_tmp4BA="Error: unexpected Array_e\n",
_tag_dyneither(_tmp4BA,sizeof(char),27))),_tag_dyneither(_tmp4B9,sizeof(void*),0)));}
exit(1);return;_LL112: {struct Cyc_Absyn_Comprehension_e_struct*_tmpFE=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmpAD;if(_tmpFE->tag != 29)goto _LL114;}_LL113:{const char*_tmp4BD;void*_tmp4BC;(
_tmp4BC=0,Cyc_fprintf(Cyc_stderr,((_tmp4BD="Error: unexpected Comprehension_e\n",
_tag_dyneither(_tmp4BD,sizeof(char),35))),_tag_dyneither(_tmp4BC,sizeof(void*),0)));}
exit(1);return;_LL114: {struct Cyc_Absyn_Aggregate_e_struct*_tmpFF=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmpAD;if(_tmpFF->tag != 30)goto _LL116;}_LL115:{const char*_tmp4C0;void*_tmp4BF;(
_tmp4BF=0,Cyc_fprintf(Cyc_stderr,((_tmp4C0="Error: unexpected Aggregate_e\n",
_tag_dyneither(_tmp4C0,sizeof(char),31))),_tag_dyneither(_tmp4BF,sizeof(void*),0)));}
exit(1);return;_LL116: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp100=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmpAD;if(_tmp100->tag != 31)goto _LL118;}_LL117:{const char*_tmp4C3;void*_tmp4C2;(
_tmp4C2=0,Cyc_fprintf(Cyc_stderr,((_tmp4C3="Error: unexpected AnonStruct_e\n",
_tag_dyneither(_tmp4C3,sizeof(char),32))),_tag_dyneither(_tmp4C2,sizeof(void*),0)));}
exit(1);return;_LL118: {struct Cyc_Absyn_Datatype_e_struct*_tmp101=(struct Cyc_Absyn_Datatype_e_struct*)
_tmpAD;if(_tmp101->tag != 32)goto _LL11A;}_LL119:{const char*_tmp4C6;void*_tmp4C5;(
_tmp4C5=0,Cyc_fprintf(Cyc_stderr,((_tmp4C6="Error: unexpected Datatype_e\n",
_tag_dyneither(_tmp4C6,sizeof(char),30))),_tag_dyneither(_tmp4C5,sizeof(void*),0)));}
exit(1);return;_LL11A: {struct Cyc_Absyn_Enum_e_struct*_tmp102=(struct Cyc_Absyn_Enum_e_struct*)
_tmpAD;if(_tmp102->tag != 33)goto _LL11C;}_LL11B:{const char*_tmp4C9;void*_tmp4C8;(
_tmp4C8=0,Cyc_fprintf(Cyc_stderr,((_tmp4C9="Error: unexpected Enum_e\n",
_tag_dyneither(_tmp4C9,sizeof(char),26))),_tag_dyneither(_tmp4C8,sizeof(void*),0)));}
exit(1);return;_LL11C: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp103=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpAD;if(_tmp103->tag != 34)goto _LLCB;}_LL11D:{const char*_tmp4CC;void*_tmp4CB;(
_tmp4CB=0,Cyc_fprintf(Cyc_stderr,((_tmp4CC="Error: unexpected AnonEnum_e\n",
_tag_dyneither(_tmp4CC,sizeof(char),30))),_tag_dyneither(_tmp4CB,sizeof(void*),0)));}
exit(1);return;_LLCB:;}void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo);void Cyc_scan_exp_opt(
struct Cyc_Absyn_Exp*eo){if((unsigned int)eo)Cyc_scan_exp(eo);return;}void Cyc_scan_type(
void*t);void Cyc_scan_type(void*t){void*_tmp12B=t;struct Cyc_Absyn_PtrInfo _tmp131;
struct Cyc_Absyn_ArrayInfo _tmp133;void*_tmp134;struct Cyc_Absyn_Exp*_tmp135;union
Cyc_Absyn_Constraint*_tmp136;struct Cyc_Absyn_FnInfo _tmp138;struct Cyc_List_List*
_tmp13A;struct Cyc_Absyn_AggrInfo _tmp13D;union Cyc_Absyn_AggrInfoU _tmp13E;struct
_tuple0*_tmp140;struct _tuple0 _tmp141;struct _dyneither_ptr*_tmp142;struct _tuple0*
_tmp144;struct _tuple0 _tmp145;struct _dyneither_ptr*_tmp146;_LL124: {struct Cyc_Absyn_VoidType_struct*
_tmp12C=(struct Cyc_Absyn_VoidType_struct*)_tmp12B;if(_tmp12C->tag != 0)goto _LL126;}
_LL125: goto _LL127;_LL126: {struct Cyc_Absyn_IntType_struct*_tmp12D=(struct Cyc_Absyn_IntType_struct*)
_tmp12B;if(_tmp12D->tag != 6)goto _LL128;}_LL127: goto _LL129;_LL128: {struct Cyc_Absyn_FloatType_struct*
_tmp12E=(struct Cyc_Absyn_FloatType_struct*)_tmp12B;if(_tmp12E->tag != 7)goto
_LL12A;}_LL129: goto _LL12B;_LL12A: {struct Cyc_Absyn_DoubleType_struct*_tmp12F=(
struct Cyc_Absyn_DoubleType_struct*)_tmp12B;if(_tmp12F->tag != 8)goto _LL12C;}
_LL12B: return;_LL12C: {struct Cyc_Absyn_PointerType_struct*_tmp130=(struct Cyc_Absyn_PointerType_struct*)
_tmp12B;if(_tmp130->tag != 5)goto _LL12E;else{_tmp131=_tmp130->f1;}}_LL12D: Cyc_scan_type(
_tmp131.elt_typ);return;_LL12E: {struct Cyc_Absyn_ArrayType_struct*_tmp132=(
struct Cyc_Absyn_ArrayType_struct*)_tmp12B;if(_tmp132->tag != 9)goto _LL130;else{
_tmp133=_tmp132->f1;_tmp134=_tmp133.elt_type;_tmp135=_tmp133.num_elts;_tmp136=
_tmp133.zero_term;}}_LL12F: Cyc_scan_type(_tmp134);Cyc_scan_exp_opt(_tmp135);
return;_LL130: {struct Cyc_Absyn_FnType_struct*_tmp137=(struct Cyc_Absyn_FnType_struct*)
_tmp12B;if(_tmp137->tag != 10)goto _LL132;else{_tmp138=_tmp137->f1;}}_LL131: Cyc_scan_type(
_tmp138.ret_typ);{struct Cyc_List_List*_tmp155=_tmp138.args;for(0;_tmp155 != 0;
_tmp155=_tmp155->tl){struct _tuple7 _tmp157;void*_tmp158;struct _tuple7*_tmp156=(
struct _tuple7*)_tmp155->hd;_tmp157=*_tmp156;_tmp158=_tmp157.f3;Cyc_scan_type(
_tmp158);}}if(_tmp138.cyc_varargs != 0)Cyc_scan_type((_tmp138.cyc_varargs)->type);
return;_LL132: {struct Cyc_Absyn_AnonAggrType_struct*_tmp139=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp12B;if(_tmp139->tag != 13)goto _LL134;else{_tmp13A=_tmp139->f2;}}_LL133: for(0;
_tmp13A != 0;_tmp13A=_tmp13A->tl){Cyc_scan_type(((struct Cyc_Absyn_Aggrfield*)
_tmp13A->hd)->type);Cyc_scan_exp_opt(((struct Cyc_Absyn_Aggrfield*)_tmp13A->hd)->width);}
return;_LL134: {struct Cyc_Absyn_AnonEnumType_struct*_tmp13B=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp12B;if(_tmp13B->tag != 15)goto _LL136;}_LL135: return;_LL136: {struct Cyc_Absyn_AggrType_struct*
_tmp13C=(struct Cyc_Absyn_AggrType_struct*)_tmp12B;if(_tmp13C->tag != 12)goto
_LL138;else{_tmp13D=_tmp13C->f1;_tmp13E=_tmp13D.aggr_info;}}_LL137: {struct
_tuple0*_tmp15A;struct _tuple0 _tmp15B;struct _dyneither_ptr*_tmp15C;struct _tuple9
_tmp159=Cyc_Absyn_aggr_kinded_name(_tmp13E);_tmp15A=_tmp159.f2;_tmp15B=*_tmp15A;
_tmp15C=_tmp15B.f2;_tmp142=_tmp15C;goto _LL139;}_LL138: {struct Cyc_Absyn_EnumType_struct*
_tmp13F=(struct Cyc_Absyn_EnumType_struct*)_tmp12B;if(_tmp13F->tag != 14)goto
_LL13A;else{_tmp140=_tmp13F->f1;_tmp141=*_tmp140;_tmp142=_tmp141.f2;}}_LL139:
_tmp146=_tmp142;goto _LL13B;_LL13A: {struct Cyc_Absyn_TypedefType_struct*_tmp143=(
struct Cyc_Absyn_TypedefType_struct*)_tmp12B;if(_tmp143->tag != 18)goto _LL13C;
else{_tmp144=_tmp143->f1;_tmp145=*_tmp144;_tmp146=_tmp145.f2;}}_LL13B: Cyc_add_target(
_tmp146);return;_LL13C: {struct Cyc_Absyn_Evar_struct*_tmp147=(struct Cyc_Absyn_Evar_struct*)
_tmp12B;if(_tmp147->tag != 1)goto _LL13E;}_LL13D:{const char*_tmp4CF;void*_tmp4CE;(
_tmp4CE=0,Cyc_fprintf(Cyc_stderr,((_tmp4CF="Error: unexpected Evar\n",
_tag_dyneither(_tmp4CF,sizeof(char),24))),_tag_dyneither(_tmp4CE,sizeof(void*),0)));}
exit(1);return;_LL13E: {struct Cyc_Absyn_VarType_struct*_tmp148=(struct Cyc_Absyn_VarType_struct*)
_tmp12B;if(_tmp148->tag != 2)goto _LL140;}_LL13F:{const char*_tmp4D2;void*_tmp4D1;(
_tmp4D1=0,Cyc_fprintf(Cyc_stderr,((_tmp4D2="Error: unexpected VarType\n",
_tag_dyneither(_tmp4D2,sizeof(char),27))),_tag_dyneither(_tmp4D1,sizeof(void*),0)));}
exit(1);return;_LL140: {struct Cyc_Absyn_DatatypeType_struct*_tmp149=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp12B;if(_tmp149->tag != 3)goto _LL142;}_LL141:{const char*_tmp4D5;void*_tmp4D4;(
_tmp4D4=0,Cyc_fprintf(Cyc_stderr,((_tmp4D5="Error: unexpected DatatypeType\n",
_tag_dyneither(_tmp4D5,sizeof(char),32))),_tag_dyneither(_tmp4D4,sizeof(void*),0)));}
exit(1);return;_LL142: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp14A=(struct
Cyc_Absyn_DatatypeFieldType_struct*)_tmp12B;if(_tmp14A->tag != 4)goto _LL144;}
_LL143:{const char*_tmp4D8;void*_tmp4D7;(_tmp4D7=0,Cyc_fprintf(Cyc_stderr,((
_tmp4D8="Error: unexpected DatatypeFieldType\n",_tag_dyneither(_tmp4D8,sizeof(
char),37))),_tag_dyneither(_tmp4D7,sizeof(void*),0)));}exit(1);return;_LL144: {
struct Cyc_Absyn_TupleType_struct*_tmp14B=(struct Cyc_Absyn_TupleType_struct*)
_tmp12B;if(_tmp14B->tag != 11)goto _LL146;}_LL145:{const char*_tmp4DB;void*_tmp4DA;(
_tmp4DA=0,Cyc_fprintf(Cyc_stderr,((_tmp4DB="Error: unexpected TupleType\n",
_tag_dyneither(_tmp4DB,sizeof(char),29))),_tag_dyneither(_tmp4DA,sizeof(void*),0)));}
exit(1);return;_LL146: {struct Cyc_Absyn_RgnHandleType_struct*_tmp14C=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp12B;if(_tmp14C->tag != 16)goto _LL148;}_LL147:{const char*_tmp4DE;void*_tmp4DD;(
_tmp4DD=0,Cyc_fprintf(Cyc_stderr,((_tmp4DE="Error: unexpected RgnHandleType\n",
_tag_dyneither(_tmp4DE,sizeof(char),33))),_tag_dyneither(_tmp4DD,sizeof(void*),0)));}
exit(1);return;_LL148: {struct Cyc_Absyn_DynRgnType_struct*_tmp14D=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp12B;if(_tmp14D->tag != 17)goto _LL14A;}_LL149:{const char*_tmp4E1;void*_tmp4E0;(
_tmp4E0=0,Cyc_fprintf(Cyc_stderr,((_tmp4E1="Error: unexpected DynRgnType\n",
_tag_dyneither(_tmp4E1,sizeof(char),30))),_tag_dyneither(_tmp4E0,sizeof(void*),0)));}
exit(1);return;_LL14A: {struct Cyc_Absyn_HeapRgn_struct*_tmp14E=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp12B;if(_tmp14E->tag != 21)goto _LL14C;}_LL14B:{const char*_tmp4E4;void*_tmp4E3;(
_tmp4E3=0,Cyc_fprintf(Cyc_stderr,((_tmp4E4="Error: unexpected HeapRgn\n",
_tag_dyneither(_tmp4E4,sizeof(char),27))),_tag_dyneither(_tmp4E3,sizeof(void*),0)));}
exit(1);return;_LL14C: {struct Cyc_Absyn_UniqueRgn_struct*_tmp14F=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp12B;if(_tmp14F->tag != 22)goto _LL14E;}_LL14D:{const char*_tmp4E7;void*_tmp4E6;(
_tmp4E6=0,Cyc_fprintf(Cyc_stderr,((_tmp4E7="Error: unexpected UniqueRgn\n",
_tag_dyneither(_tmp4E7,sizeof(char),29))),_tag_dyneither(_tmp4E6,sizeof(void*),0)));}
exit(1);return;_LL14E: {struct Cyc_Absyn_AccessEff_struct*_tmp150=(struct Cyc_Absyn_AccessEff_struct*)
_tmp12B;if(_tmp150->tag != 23)goto _LL150;}_LL14F:{const char*_tmp4EA;void*_tmp4E9;(
_tmp4E9=0,Cyc_fprintf(Cyc_stderr,((_tmp4EA="Error: unexpected AccessEff\n",
_tag_dyneither(_tmp4EA,sizeof(char),29))),_tag_dyneither(_tmp4E9,sizeof(void*),0)));}
exit(1);return;_LL150: {struct Cyc_Absyn_JoinEff_struct*_tmp151=(struct Cyc_Absyn_JoinEff_struct*)
_tmp12B;if(_tmp151->tag != 24)goto _LL152;}_LL151:{const char*_tmp4ED;void*_tmp4EC;(
_tmp4EC=0,Cyc_fprintf(Cyc_stderr,((_tmp4ED="Error: unexpected JoinEff\n",
_tag_dyneither(_tmp4ED,sizeof(char),27))),_tag_dyneither(_tmp4EC,sizeof(void*),0)));}
exit(1);return;_LL152: {struct Cyc_Absyn_RgnsEff_struct*_tmp152=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp12B;if(_tmp152->tag != 25)goto _LL154;}_LL153:{const char*_tmp4F0;void*_tmp4EF;(
_tmp4EF=0,Cyc_fprintf(Cyc_stderr,((_tmp4F0="Error: unexpected RgnsEff\n",
_tag_dyneither(_tmp4F0,sizeof(char),27))),_tag_dyneither(_tmp4EF,sizeof(void*),0)));}
exit(1);return;_LL154: {struct Cyc_Absyn_TagType_struct*_tmp153=(struct Cyc_Absyn_TagType_struct*)
_tmp12B;if(_tmp153->tag != 20)goto _LL156;}_LL155:{const char*_tmp4F3;void*_tmp4F2;(
_tmp4F2=0,Cyc_fprintf(Cyc_stderr,((_tmp4F3="Error: unexpected tag_t\n",
_tag_dyneither(_tmp4F3,sizeof(char),25))),_tag_dyneither(_tmp4F2,sizeof(void*),0)));}
exit(1);return;_LL156: {struct Cyc_Absyn_ValueofType_struct*_tmp154=(struct Cyc_Absyn_ValueofType_struct*)
_tmp12B;if(_tmp154->tag != 19)goto _LL123;}_LL157:{const char*_tmp4F6;void*_tmp4F5;(
_tmp4F5=0,Cyc_fprintf(Cyc_stderr,((_tmp4F6="Error: unexpected valueof_t\n",
_tag_dyneither(_tmp4F6,sizeof(char),29))),_tag_dyneither(_tmp4F5,sizeof(void*),0)));}
exit(1);return;_LL123:;}struct _tuple22{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct _tuple17*Cyc_scan_decl(struct Cyc_Absyn_Decl*d);struct _tuple17*
Cyc_scan_decl(struct Cyc_Absyn_Decl*d){{struct Cyc_Set_Set**_tmp4F7;Cyc_current_targets=((
_tmp4F7=_cycalloc(sizeof(*_tmp4F7)),((_tmp4F7[0]=((struct Cyc_Set_Set*(*)(int(*
cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),
_tmp4F7))));}{void*_tmp17A=d->r;struct Cyc_Absyn_Vardecl*_tmp17C;struct Cyc_Absyn_Fndecl*
_tmp17E;struct Cyc_Absyn_Aggrdecl*_tmp180;struct Cyc_Absyn_Enumdecl*_tmp182;struct
Cyc_Absyn_Typedefdecl*_tmp184;_LL159: {struct Cyc_Absyn_Var_d_struct*_tmp17B=(
struct Cyc_Absyn_Var_d_struct*)_tmp17A;if(_tmp17B->tag != 0)goto _LL15B;else{
_tmp17C=_tmp17B->f1;}}_LL15A: {struct _tuple0 _tmp191;struct _dyneither_ptr*_tmp192;
struct _tuple0*_tmp190=_tmp17C->name;_tmp191=*_tmp190;_tmp192=_tmp191.f2;Cyc_current_source=(
struct _dyneither_ptr*)_tmp192;Cyc_scan_type(_tmp17C->type);Cyc_scan_exp_opt(
_tmp17C->initializer);goto _LL158;}_LL15B: {struct Cyc_Absyn_Fn_d_struct*_tmp17D=(
struct Cyc_Absyn_Fn_d_struct*)_tmp17A;if(_tmp17D->tag != 1)goto _LL15D;else{_tmp17E=
_tmp17D->f1;}}_LL15C: {struct _tuple0 _tmp194;struct _dyneither_ptr*_tmp195;struct
_tuple0*_tmp193=_tmp17E->name;_tmp194=*_tmp193;_tmp195=_tmp194.f2;Cyc_current_source=(
struct _dyneither_ptr*)_tmp195;Cyc_scan_type(_tmp17E->ret_type);{struct Cyc_List_List*
_tmp196=_tmp17E->args;for(0;_tmp196 != 0;_tmp196=_tmp196->tl){struct _tuple22
_tmp198;void*_tmp199;struct _tuple22*_tmp197=(struct _tuple22*)_tmp196->hd;_tmp198=*
_tmp197;_tmp199=_tmp198.f3;Cyc_scan_type(_tmp199);}}if(_tmp17E->cyc_varargs != 0)
Cyc_scan_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp17E->cyc_varargs))->type);
if(_tmp17E->is_inline){const char*_tmp4FA;void*_tmp4F9;(_tmp4F9=0,Cyc_fprintf(Cyc_stderr,((
_tmp4FA="Warning: ignoring inline function\n",_tag_dyneither(_tmp4FA,sizeof(char),
35))),_tag_dyneither(_tmp4F9,sizeof(void*),0)));}goto _LL158;}_LL15D: {struct Cyc_Absyn_Aggr_d_struct*
_tmp17F=(struct Cyc_Absyn_Aggr_d_struct*)_tmp17A;if(_tmp17F->tag != 6)goto _LL15F;
else{_tmp180=_tmp17F->f1;}}_LL15E: {struct _tuple0 _tmp19D;struct _dyneither_ptr*
_tmp19E;struct _tuple0*_tmp19C=_tmp180->name;_tmp19D=*_tmp19C;_tmp19E=_tmp19D.f2;
Cyc_current_source=(struct _dyneither_ptr*)_tmp19E;if((unsigned int)_tmp180->impl){{
struct Cyc_List_List*_tmp19F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp180->impl))->fields;
for(0;_tmp19F != 0;_tmp19F=_tmp19F->tl){struct Cyc_Absyn_Aggrfield*_tmp1A0=(struct
Cyc_Absyn_Aggrfield*)_tmp19F->hd;Cyc_scan_type(_tmp1A0->type);Cyc_scan_exp_opt(
_tmp1A0->width);}}{struct Cyc_List_List*_tmp1A1=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp180->impl))->fields;for(0;_tmp1A1 != 0;_tmp1A1=_tmp1A1->tl){;}};}
goto _LL158;}_LL15F: {struct Cyc_Absyn_Enum_d_struct*_tmp181=(struct Cyc_Absyn_Enum_d_struct*)
_tmp17A;if(_tmp181->tag != 8)goto _LL161;else{_tmp182=_tmp181->f1;}}_LL160: {
struct _tuple0 _tmp1A3;struct _dyneither_ptr*_tmp1A4;struct _tuple0*_tmp1A2=_tmp182->name;
_tmp1A3=*_tmp1A2;_tmp1A4=_tmp1A3.f2;Cyc_current_source=(struct _dyneither_ptr*)
_tmp1A4;if((unsigned int)_tmp182->fields){{struct Cyc_List_List*_tmp1A5=(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp182->fields))->v;for(0;
_tmp1A5 != 0;_tmp1A5=_tmp1A5->tl){struct Cyc_Absyn_Enumfield*_tmp1A6=(struct Cyc_Absyn_Enumfield*)
_tmp1A5->hd;Cyc_scan_exp_opt(_tmp1A6->tag);}}{struct Cyc_List_List*_tmp1A7=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp182->fields))->v;for(
0;_tmp1A7 != 0;_tmp1A7=_tmp1A7->tl){;}};}goto _LL158;}_LL161: {struct Cyc_Absyn_Typedef_d_struct*
_tmp183=(struct Cyc_Absyn_Typedef_d_struct*)_tmp17A;if(_tmp183->tag != 9)goto
_LL163;else{_tmp184=_tmp183->f1;}}_LL162: {struct _tuple0 _tmp1A9;struct
_dyneither_ptr*_tmp1AA;struct _tuple0*_tmp1A8=_tmp184->name;_tmp1A9=*_tmp1A8;
_tmp1AA=_tmp1A9.f2;Cyc_current_source=(struct _dyneither_ptr*)_tmp1AA;if((
unsigned int)_tmp184->defn)Cyc_scan_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp184->defn))->v);goto _LL158;}_LL163: {struct Cyc_Absyn_Region_d_struct*
_tmp185=(struct Cyc_Absyn_Region_d_struct*)_tmp17A;if(_tmp185->tag != 4)goto _LL165;}
_LL164:{const char*_tmp4FD;void*_tmp4FC;(_tmp4FC=0,Cyc_fprintf(Cyc_stderr,((
_tmp4FD="Error: unexpected region declaration",_tag_dyneither(_tmp4FD,sizeof(
char),37))),_tag_dyneither(_tmp4FC,sizeof(void*),0)));}exit(1);_LL165: {struct
Cyc_Absyn_Alias_d_struct*_tmp186=(struct Cyc_Absyn_Alias_d_struct*)_tmp17A;if(
_tmp186->tag != 5)goto _LL167;}_LL166:{const char*_tmp500;void*_tmp4FF;(_tmp4FF=0,
Cyc_fprintf(Cyc_stderr,((_tmp500="Error: unexpected alias declaration",
_tag_dyneither(_tmp500,sizeof(char),36))),_tag_dyneither(_tmp4FF,sizeof(void*),0)));}
exit(1);_LL167: {struct Cyc_Absyn_Porton_d_struct*_tmp187=(struct Cyc_Absyn_Porton_d_struct*)
_tmp17A;if(_tmp187->tag != 14)goto _LL169;}_LL168:{const char*_tmp503;void*_tmp502;(
_tmp502=0,Cyc_fprintf(Cyc_stderr,((_tmp503="Error: unexpected __cyclone_port_on__",
_tag_dyneither(_tmp503,sizeof(char),38))),_tag_dyneither(_tmp502,sizeof(void*),0)));}
exit(1);return 0;_LL169: {struct Cyc_Absyn_Portoff_d_struct*_tmp188=(struct Cyc_Absyn_Portoff_d_struct*)
_tmp17A;if(_tmp188->tag != 15)goto _LL16B;}_LL16A:{const char*_tmp506;void*_tmp505;(
_tmp505=0,Cyc_fprintf(Cyc_stderr,((_tmp506="Error: unexpected __cyclone_port_off__",
_tag_dyneither(_tmp506,sizeof(char),39))),_tag_dyneither(_tmp505,sizeof(void*),0)));}
exit(1);return 0;_LL16B: {struct Cyc_Absyn_Let_d_struct*_tmp189=(struct Cyc_Absyn_Let_d_struct*)
_tmp17A;if(_tmp189->tag != 2)goto _LL16D;}_LL16C:{const char*_tmp509;void*_tmp508;(
_tmp508=0,Cyc_fprintf(Cyc_stderr,((_tmp509="Error: unexpected let declaration\n",
_tag_dyneither(_tmp509,sizeof(char),35))),_tag_dyneither(_tmp508,sizeof(void*),0)));}
exit(1);return 0;_LL16D: {struct Cyc_Absyn_Datatype_d_struct*_tmp18A=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp17A;if(_tmp18A->tag != 7)goto _LL16F;}_LL16E:{const char*_tmp50C;void*_tmp50B;(
_tmp50B=0,Cyc_fprintf(Cyc_stderr,((_tmp50C="Error: unexpected datatype declaration\n",
_tag_dyneither(_tmp50C,sizeof(char),40))),_tag_dyneither(_tmp50B,sizeof(void*),0)));}
exit(1);return 0;_LL16F: {struct Cyc_Absyn_Letv_d_struct*_tmp18B=(struct Cyc_Absyn_Letv_d_struct*)
_tmp17A;if(_tmp18B->tag != 3)goto _LL171;}_LL170:{const char*_tmp50F;void*_tmp50E;(
_tmp50E=0,Cyc_fprintf(Cyc_stderr,((_tmp50F="Error: unexpected let declaration\n",
_tag_dyneither(_tmp50F,sizeof(char),35))),_tag_dyneither(_tmp50E,sizeof(void*),0)));}
exit(1);return 0;_LL171: {struct Cyc_Absyn_Namespace_d_struct*_tmp18C=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp17A;if(_tmp18C->tag != 10)goto _LL173;}_LL172:{const char*_tmp512;void*_tmp511;(
_tmp511=0,Cyc_fprintf(Cyc_stderr,((_tmp512="Error: unexpected namespace declaration\n",
_tag_dyneither(_tmp512,sizeof(char),41))),_tag_dyneither(_tmp511,sizeof(void*),0)));}
exit(1);return 0;_LL173: {struct Cyc_Absyn_Using_d_struct*_tmp18D=(struct Cyc_Absyn_Using_d_struct*)
_tmp17A;if(_tmp18D->tag != 11)goto _LL175;}_LL174:{const char*_tmp515;void*_tmp514;(
_tmp514=0,Cyc_fprintf(Cyc_stderr,((_tmp515="Error: unexpected using declaration\n",
_tag_dyneither(_tmp515,sizeof(char),37))),_tag_dyneither(_tmp514,sizeof(void*),0)));}
exit(1);return 0;_LL175: {struct Cyc_Absyn_ExternC_d_struct*_tmp18E=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp17A;if(_tmp18E->tag != 12)goto _LL177;}_LL176:{const char*_tmp518;void*_tmp517;(
_tmp517=0,Cyc_fprintf(Cyc_stderr,((_tmp518="Error: unexpected extern \"C\" declaration\n",
_tag_dyneither(_tmp518,sizeof(char),42))),_tag_dyneither(_tmp517,sizeof(void*),0)));}
exit(1);return 0;_LL177: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp18F=(struct
Cyc_Absyn_ExternCinclude_d_struct*)_tmp17A;if(_tmp18F->tag != 13)goto _LL158;}
_LL178:{const char*_tmp51B;void*_tmp51A;(_tmp51A=0,Cyc_fprintf(Cyc_stderr,((
_tmp51B="Error: unexpected extern \"C include\" declaration\n",_tag_dyneither(
_tmp51B,sizeof(char),50))),_tag_dyneither(_tmp51A,sizeof(void*),0)));}exit(1);
return 0;_LL158:;}{struct _tuple17*_tmp51C;return(_tmp51C=_cycalloc(sizeof(*
_tmp51C)),((_tmp51C->f1=(struct _dyneither_ptr*)_check_null(Cyc_current_source),((
_tmp51C->f2=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),_tmp51C)))));};}
struct Cyc_Hashtable_Table*Cyc_new_deps();struct Cyc_Hashtable_Table*Cyc_new_deps(){
return((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*),int(*hash)(struct _dyneither_ptr*)))Cyc_Hashtable_create)(
107,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);}struct Cyc_Set_Set*Cyc_find(
struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*x);struct Cyc_Set_Set*Cyc_find(
struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*x){struct _handler_cons _tmp1C2;
_push_handler(& _tmp1C2);{int _tmp1C4=0;if(setjmp(_tmp1C2.handler))_tmp1C4=1;if(!
_tmp1C4){{struct Cyc_Set_Set*_tmp1C5=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*
t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(t,x);_npop_handler(0);return
_tmp1C5;};_pop_handler();}else{void*_tmp1C3=(void*)_exn_thrown;void*_tmp1C7=
_tmp1C3;_LL17A: {struct Cyc_Core_Not_found_struct*_tmp1C8=(struct Cyc_Core_Not_found_struct*)
_tmp1C7;if(_tmp1C8->tag != Cyc_Core_Not_found)goto _LL17C;}_LL17B: return((struct
Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(
Cyc_strptrcmp);_LL17C:;_LL17D:(void)_throw(_tmp1C7);_LL179:;}};}struct Cyc_Set_Set*
Cyc_reachable(struct Cyc_List_List*init,struct Cyc_Hashtable_Table*t);struct Cyc_Set_Set*
Cyc_reachable(struct Cyc_List_List*init,struct Cyc_Hashtable_Table*t){struct Cyc_Set_Set*
emptyset=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct
_dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);struct Cyc_Set_Set*curr;for(curr=
emptyset;init != 0;init=init->tl){curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*
s,struct _dyneither_ptr*elt))Cyc_Set_insert)(curr,(struct _dyneither_ptr*)init->hd);}{
struct Cyc_Set_Set*_tmp1C9=curr;struct _dyneither_ptr*_tmp1CA=_init_dyneither_ptr(
_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1);while(((int(*)(struct
Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp1C9)> 0){struct Cyc_Set_Set*_tmp1CB=
emptyset;struct Cyc_Iter_Iter _tmp1CC=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*
rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,_tmp1C9);while(((
int(*)(struct Cyc_Iter_Iter,struct _dyneither_ptr**))Cyc_Iter_next)(_tmp1CC,&
_tmp1CA)){_tmp1CB=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*
s2))Cyc_Set_union_two)(_tmp1CB,Cyc_find(t,_tmp1CA));}_tmp1C9=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_diff)(_tmp1CB,curr);curr=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
curr,_tmp1C9);}return curr;};}enum Cyc_buildlib_mode{Cyc_NORMAL  = 0,Cyc_GATHER  = 
1,Cyc_GATHERSCRIPT  = 2,Cyc_FINISH  = 3};static enum Cyc_buildlib_mode Cyc_mode=Cyc_NORMAL;
static int Cyc_gathering();static int Cyc_gathering(){return Cyc_mode == Cyc_GATHER
 || Cyc_mode == Cyc_GATHERSCRIPT;}static struct Cyc___cycFILE*Cyc_script_file=0;int
Cyc_prscript(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);int Cyc_prscript(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap){if(Cyc_script_file == 0){{const
char*_tmp51F;void*_tmp51E;(_tmp51E=0,Cyc_fprintf(Cyc_stderr,((_tmp51F="Internal error: script file is NULL\n",
_tag_dyneither(_tmp51F,sizeof(char),37))),_tag_dyneither(_tmp51E,sizeof(void*),0)));}
exit(1);}return Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_script_file),
fmt,ap);}int Cyc_force_directory(struct _dyneither_ptr d);int Cyc_force_directory(
struct _dyneither_ptr d){if(Cyc_mode == Cyc_GATHERSCRIPT){const char*_tmp524;void*
_tmp523[2];struct Cyc_String_pa_struct _tmp522;struct Cyc_String_pa_struct _tmp521;(
_tmp521.tag=0,((_tmp521.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d),((
_tmp522.tag=0,((_tmp522.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d),((
_tmp523[0]=& _tmp522,((_tmp523[1]=& _tmp521,Cyc_prscript(((_tmp524="if ! test -e %s; then mkdir %s; fi\n",
_tag_dyneither(_tmp524,sizeof(char),36))),_tag_dyneither(_tmp523,sizeof(void*),2)))))))))))));}
else{unsigned short _tmp525[0];int _tmp1D4=Cyc_open((const char*)_check_null(
_untag_dyneither_ptr(d,sizeof(char),1)),0,_tag_dyneither(_tmp525,sizeof(
unsigned short),0));if(_tmp1D4 == - 1){if(mkdir((const char*)_untag_dyneither_ptr(d,
sizeof(char),1),448)== - 1){{const char*_tmp529;void*_tmp528[1];struct Cyc_String_pa_struct
_tmp527;(_tmp527.tag=0,((_tmp527.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
d),((_tmp528[0]=& _tmp527,Cyc_fprintf(Cyc_stderr,((_tmp529="Error: could not create directory %s\n",
_tag_dyneither(_tmp529,sizeof(char),38))),_tag_dyneither(_tmp528,sizeof(void*),1)))))));}
return 1;}}else{close(_tmp1D4);}}return 0;}int Cyc_force_directory_prefixes(struct
_dyneither_ptr file);int Cyc_force_directory_prefixes(struct _dyneither_ptr file){
struct _dyneither_ptr _tmp1D9=Cyc_strdup((struct _dyneither_ptr)file);struct Cyc_List_List*
_tmp1DA=0;while(1){_tmp1D9=Cyc_Filename_dirname((struct _dyneither_ptr)_tmp1D9);
if(Cyc_strlen((struct _dyneither_ptr)_tmp1D9)== 0)break;{struct _dyneither_ptr*
_tmp52C;struct Cyc_List_List*_tmp52B;_tmp1DA=((_tmp52B=_cycalloc(sizeof(*_tmp52B)),((
_tmp52B->hd=((_tmp52C=_cycalloc(sizeof(*_tmp52C)),((_tmp52C[0]=(struct
_dyneither_ptr)_tmp1D9,_tmp52C)))),((_tmp52B->tl=_tmp1DA,_tmp52B))))));};}for(0;
_tmp1DA != 0;_tmp1DA=_tmp1DA->tl){if(Cyc_force_directory(*((struct _dyneither_ptr*)
_tmp1DA->hd)))return 1;}return 0;}char Cyc_NO_SUPPORT[11]="NO_SUPPORT";struct Cyc_NO_SUPPORT_struct{
char*tag;struct _dyneither_ptr f1;};struct _tuple23{struct Cyc_List_List*f1;struct
Cyc_List_List*f2;};int Cyc_process_file(const char*filename,struct Cyc_List_List*
start_symbols,struct Cyc_List_List*omit_symbols,struct Cyc_List_List*hstubs,struct
Cyc_List_List*cstubs,struct Cyc_List_List*cycstubs);int Cyc_process_file(const char*
filename,struct Cyc_List_List*start_symbols,struct Cyc_List_List*omit_symbols,
struct Cyc_List_List*hstubs,struct Cyc_List_List*cstubs,struct Cyc_List_List*
cycstubs){struct Cyc___cycFILE*maybe;struct Cyc___cycFILE*in_file;struct Cyc___cycFILE*
out_file;int errorcode=0;{const char*_tmp532;void*_tmp531[1];const char*_tmp530;
struct Cyc_String_pa_struct _tmp52F;(_tmp52F.tag=0,((_tmp52F.f1=(struct
_dyneither_ptr)((_tmp530=filename,_tag_dyneither(_tmp530,sizeof(char),
_get_zero_arr_size_char(_tmp530,1)))),((_tmp531[0]=& _tmp52F,Cyc_fprintf(Cyc_stderr,((
_tmp532="********************************* %s...\n",_tag_dyneither(_tmp532,
sizeof(char),41))),_tag_dyneither(_tmp531,sizeof(void*),1)))))));}if(!Cyc_gathering()){
const char*_tmp538;void*_tmp537[1];const char*_tmp536;struct Cyc_String_pa_struct
_tmp535;(_tmp535.tag=0,((_tmp535.f1=(struct _dyneither_ptr)((_tmp536=filename,
_tag_dyneither(_tmp536,sizeof(char),_get_zero_arr_size_char(_tmp536,1)))),((
_tmp537[0]=& _tmp535,Cyc_log(((_tmp538="\n%s:\n",_tag_dyneither(_tmp538,sizeof(
char),6))),_tag_dyneither(_tmp537,sizeof(void*),1)))))));}{const char*_tmp539;
struct _dyneither_ptr _tmp1E5=Cyc_Filename_basename(((_tmp539=filename,
_tag_dyneither(_tmp539,sizeof(char),_get_zero_arr_size_char(_tmp539,1)))));const
char*_tmp53A;struct _dyneither_ptr _tmp1E6=Cyc_Filename_dirname(((_tmp53A=filename,
_tag_dyneither(_tmp53A,sizeof(char),_get_zero_arr_size_char(_tmp53A,1)))));
struct _dyneither_ptr _tmp1E7=Cyc_Filename_chop_extension((struct _dyneither_ptr)
_tmp1E5);const char*_tmp53B;const char*_tmp1E8=(const char*)_untag_dyneither_ptr(
Cyc_strconcat((struct _dyneither_ptr)_tmp1E7,((_tmp53B=".iA",_tag_dyneither(
_tmp53B,sizeof(char),4)))),sizeof(char),1);const char*_tmp543;void*_tmp542[1];
struct Cyc_String_pa_struct _tmp541;const char*_tmp53F;void*_tmp53E[1];struct Cyc_String_pa_struct
_tmp53D;const char*_tmp1E9=(const char*)_untag_dyneither_ptr(_get_dyneither_size(
_tmp1E6,sizeof(char))== 0?(_tmp541.tag=0,((_tmp541.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp1E7),((_tmp542[0]=& _tmp541,Cyc_aprintf(((_tmp543="%s.iB",
_tag_dyneither(_tmp543,sizeof(char),6))),_tag_dyneither(_tmp542,sizeof(void*),1))))))):
Cyc_Filename_concat((struct _dyneither_ptr)_tmp1E6,(struct _dyneither_ptr)((
_tmp53D.tag=0,((_tmp53D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1E7),((
_tmp53E[0]=& _tmp53D,Cyc_aprintf(((_tmp53F="%s.iB",_tag_dyneither(_tmp53F,sizeof(
char),6))),_tag_dyneither(_tmp53E,sizeof(void*),1))))))))),sizeof(char),1);const
char*_tmp54B;void*_tmp54A[1];struct Cyc_String_pa_struct _tmp549;const char*_tmp547;
void*_tmp546[1];struct Cyc_String_pa_struct _tmp545;const char*_tmp1EA=(const char*)
_untag_dyneither_ptr(_get_dyneither_size(_tmp1E6,sizeof(char))== 0?(_tmp549.tag=
0,((_tmp549.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1E7),((_tmp54A[0]=&
_tmp549,Cyc_aprintf(((_tmp54B="%s.iC",_tag_dyneither(_tmp54B,sizeof(char),6))),
_tag_dyneither(_tmp54A,sizeof(void*),1))))))): Cyc_Filename_concat((struct
_dyneither_ptr)_tmp1E6,(struct _dyneither_ptr)((_tmp545.tag=0,((_tmp545.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1E7),((_tmp546[0]=& _tmp545,Cyc_aprintf(((
_tmp547="%s.iC",_tag_dyneither(_tmp547,sizeof(char),6))),_tag_dyneither(_tmp546,
sizeof(void*),1))))))))),sizeof(char),1);const char*_tmp553;void*_tmp552[1];
struct Cyc_String_pa_struct _tmp551;const char*_tmp54F;void*_tmp54E[1];struct Cyc_String_pa_struct
_tmp54D;const char*_tmp1EB=(const char*)_untag_dyneither_ptr(_get_dyneither_size(
_tmp1E6,sizeof(char))== 0?(_tmp551.tag=0,((_tmp551.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp1E7),((_tmp552[0]=& _tmp551,Cyc_aprintf(((_tmp553="%s.iD",
_tag_dyneither(_tmp553,sizeof(char),6))),_tag_dyneither(_tmp552,sizeof(void*),1))))))):
Cyc_Filename_concat((struct _dyneither_ptr)_tmp1E6,(struct _dyneither_ptr)((
_tmp54D.tag=0,((_tmp54D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1E7),((
_tmp54E[0]=& _tmp54D,Cyc_aprintf(((_tmp54F="%s.iD",_tag_dyneither(_tmp54F,sizeof(
char),6))),_tag_dyneither(_tmp54E,sizeof(void*),1))))))))),sizeof(char),1);
struct _handler_cons _tmp1EC;_push_handler(& _tmp1EC);{int _tmp1EE=0;if(setjmp(
_tmp1EC.handler))_tmp1EE=1;if(!_tmp1EE){{const char*_tmp554;if(Cyc_force_directory_prefixes(((
_tmp554=filename,_tag_dyneither(_tmp554,sizeof(char),_get_zero_arr_size_char(
_tmp554,1)))))){int _tmp1F0=1;_npop_handler(0);return _tmp1F0;}}if(Cyc_mode != Cyc_FINISH){
Cyc_current_cpp=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_current_cpp);if(Cyc_mode == Cyc_GATHERSCRIPT){{const char*_tmp55A;void*_tmp559[
1];const char*_tmp558;struct Cyc_String_pa_struct _tmp557;(_tmp557.tag=0,((_tmp557.f1=(
struct _dyneither_ptr)((_tmp558=_tmp1E8,_tag_dyneither(_tmp558,sizeof(char),
_get_zero_arr_size_char(_tmp558,1)))),((_tmp559[0]=& _tmp557,Cyc_prscript(((
_tmp55A="cat >%s <<XXX\n",_tag_dyneither(_tmp55A,sizeof(char),15))),
_tag_dyneither(_tmp559,sizeof(void*),1)))))));}{struct Cyc_List_List*_tmp1F5=Cyc_current_cpp;
for(0;_tmp1F5 != 0;_tmp1F5=_tmp1F5->tl){const char*_tmp55E;void*_tmp55D[1];struct
Cyc_String_pa_struct _tmp55C;(_tmp55C.tag=0,((_tmp55C.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp1F5->hd)),((_tmp55D[0]=&
_tmp55C,Cyc_prscript(((_tmp55E="%s",_tag_dyneither(_tmp55E,sizeof(char),3))),
_tag_dyneither(_tmp55D,sizeof(void*),1)))))));}}{const char*_tmp564;void*_tmp563[
1];const char*_tmp562;struct Cyc_String_pa_struct _tmp561;(_tmp561.tag=0,((_tmp561.f1=(
struct _dyneither_ptr)((_tmp562=filename,_tag_dyneither(_tmp562,sizeof(char),
_get_zero_arr_size_char(_tmp562,1)))),((_tmp563[0]=& _tmp561,Cyc_prscript(((
_tmp564="#include <%s>\n",_tag_dyneither(_tmp564,sizeof(char),15))),
_tag_dyneither(_tmp563,sizeof(void*),1)))))));}{const char*_tmp567;void*_tmp566;(
_tmp566=0,Cyc_prscript(((_tmp567="XXX\n",_tag_dyneither(_tmp567,sizeof(char),5))),
_tag_dyneither(_tmp566,sizeof(void*),0)));}{const char*_tmp570;void*_tmp56F[2];
const char*_tmp56E;struct Cyc_String_pa_struct _tmp56D;const char*_tmp56C;struct Cyc_String_pa_struct
_tmp56B;(_tmp56B.tag=0,((_tmp56B.f1=(struct _dyneither_ptr)((_tmp56C=_tmp1E8,
_tag_dyneither(_tmp56C,sizeof(char),_get_zero_arr_size_char(_tmp56C,1)))),((
_tmp56D.tag=0,((_tmp56D.f1=(struct _dyneither_ptr)((_tmp56E=_tmp1E9,
_tag_dyneither(_tmp56E,sizeof(char),_get_zero_arr_size_char(_tmp56E,1)))),((
_tmp56F[0]=& _tmp56D,((_tmp56F[1]=& _tmp56B,Cyc_prscript(((_tmp570="$GCC -E -dM -o %s -x c %s && \\\n",
_tag_dyneither(_tmp570,sizeof(char),32))),_tag_dyneither(_tmp56F,sizeof(void*),2)))))))))))));}{
const char*_tmp579;void*_tmp578[2];const char*_tmp577;struct Cyc_String_pa_struct
_tmp576;const char*_tmp575;struct Cyc_String_pa_struct _tmp574;(_tmp574.tag=0,((
_tmp574.f1=(struct _dyneither_ptr)((_tmp575=_tmp1E8,_tag_dyneither(_tmp575,
sizeof(char),_get_zero_arr_size_char(_tmp575,1)))),((_tmp576.tag=0,((_tmp576.f1=(
struct _dyneither_ptr)((_tmp577=_tmp1EA,_tag_dyneither(_tmp577,sizeof(char),
_get_zero_arr_size_char(_tmp577,1)))),((_tmp578[0]=& _tmp576,((_tmp578[1]=&
_tmp574,Cyc_prscript(((_tmp579="$GCC -E     -o %s -x c %s;\n",_tag_dyneither(
_tmp579,sizeof(char),28))),_tag_dyneither(_tmp578,sizeof(void*),2)))))))))))));}{
const char*_tmp57F;void*_tmp57E[1];const char*_tmp57D;struct Cyc_String_pa_struct
_tmp57C;(_tmp57C.tag=0,((_tmp57C.f1=(struct _dyneither_ptr)((_tmp57D=_tmp1E8,
_tag_dyneither(_tmp57D,sizeof(char),_get_zero_arr_size_char(_tmp57D,1)))),((
_tmp57E[0]=& _tmp57C,Cyc_prscript(((_tmp57F="rm %s\n",_tag_dyneither(_tmp57F,
sizeof(char),7))),_tag_dyneither(_tmp57E,sizeof(void*),1)))))));};}else{maybe=
Cyc_fopen(_tmp1E8,(const char*)"w");if(!((unsigned int)maybe)){{const char*_tmp585;
void*_tmp584[1];const char*_tmp583;struct Cyc_String_pa_struct _tmp582;(_tmp582.tag=
0,((_tmp582.f1=(struct _dyneither_ptr)((_tmp583=_tmp1E8,_tag_dyneither(_tmp583,
sizeof(char),_get_zero_arr_size_char(_tmp583,1)))),((_tmp584[0]=& _tmp582,Cyc_fprintf(
Cyc_stderr,((_tmp585="Error: could not create file %s\n",_tag_dyneither(_tmp585,
sizeof(char),33))),_tag_dyneither(_tmp584,sizeof(void*),1)))))));}{int _tmp213=1;
_npop_handler(0);return _tmp213;};}out_file=(struct Cyc___cycFILE*)maybe;{struct
Cyc_List_List*_tmp214=Cyc_current_cpp;for(0;_tmp214 != 0;_tmp214=_tmp214->tl){Cyc_fputs((
const char*)_untag_dyneither_ptr(*((struct _dyneither_ptr*)_tmp214->hd),sizeof(
char),1),out_file);}}{const char*_tmp58B;void*_tmp58A[1];const char*_tmp589;struct
Cyc_String_pa_struct _tmp588;(_tmp588.tag=0,((_tmp588.f1=(struct _dyneither_ptr)((
_tmp589=filename,_tag_dyneither(_tmp589,sizeof(char),_get_zero_arr_size_char(
_tmp589,1)))),((_tmp58A[0]=& _tmp588,Cyc_fprintf(out_file,((_tmp58B="#include <%s>\n",
_tag_dyneither(_tmp58B,sizeof(char),15))),_tag_dyneither(_tmp58A,sizeof(void*),1)))))));}
Cyc_fclose(out_file);{struct _dyneither_ptr _tmp219=Cstring_to_string(Ccomp);const
char*_tmp595;void*_tmp594[3];struct Cyc_String_pa_struct _tmp593;const char*_tmp592;
struct Cyc_String_pa_struct _tmp591;const char*_tmp590;struct Cyc_String_pa_struct
_tmp58F;char*cmd=(char*)_untag_dyneither_ptr(((_tmp58F.tag=0,((_tmp58F.f1=(
struct _dyneither_ptr)((_tmp590=_tmp1E8,_tag_dyneither(_tmp590,sizeof(char),
_get_zero_arr_size_char(_tmp590,1)))),((_tmp591.tag=0,((_tmp591.f1=(struct
_dyneither_ptr)((_tmp592=_tmp1E9,_tag_dyneither(_tmp592,sizeof(char),
_get_zero_arr_size_char(_tmp592,1)))),((_tmp593.tag=0,((_tmp593.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp219),((_tmp594[0]=& _tmp593,((_tmp594[1]=&
_tmp591,((_tmp594[2]=& _tmp58F,Cyc_aprintf(((_tmp595="%s -E -dM -o %s -x c %s",
_tag_dyneither(_tmp595,sizeof(char),24))),_tag_dyneither(_tmp594,sizeof(void*),3)))))))))))))))))))),
sizeof(char),1);if(!system((const char*)cmd)){{const char*_tmp59F;void*_tmp59E[3];
struct Cyc_String_pa_struct _tmp59D;const char*_tmp59C;struct Cyc_String_pa_struct
_tmp59B;const char*_tmp59A;struct Cyc_String_pa_struct _tmp599;cmd=(char*)
_untag_dyneither_ptr(((_tmp599.tag=0,((_tmp599.f1=(struct _dyneither_ptr)((
_tmp59A=_tmp1E8,_tag_dyneither(_tmp59A,sizeof(char),_get_zero_arr_size_char(
_tmp59A,1)))),((_tmp59B.tag=0,((_tmp59B.f1=(struct _dyneither_ptr)((_tmp59C=
_tmp1EA,_tag_dyneither(_tmp59C,sizeof(char),_get_zero_arr_size_char(_tmp59C,1)))),((
_tmp59D.tag=0,((_tmp59D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp219),((
_tmp59E[0]=& _tmp59D,((_tmp59E[1]=& _tmp59B,((_tmp59E[2]=& _tmp599,Cyc_aprintf(((
_tmp59F="%s -E -o %s -x c %s",_tag_dyneither(_tmp59F,sizeof(char),20))),
_tag_dyneither(_tmp59E,sizeof(void*),3)))))))))))))))))))),sizeof(char),1);}
system((const char*)cmd);}remove(_tmp1E8);};}}if(Cyc_gathering()){int _tmp228=0;
_npop_handler(0);return _tmp228;}{struct Cyc_Hashtable_Table*t=Cyc_new_deps();
maybe=Cyc_fopen(_tmp1E9,(const char*)"r");if(!((unsigned int)maybe)){struct Cyc_NO_SUPPORT_struct
_tmp5B0;const char*_tmp5AF;void*_tmp5AE[1];const char*_tmp5AD;struct Cyc_String_pa_struct
_tmp5AC;struct Cyc_NO_SUPPORT_struct*_tmp5AB;(int)_throw((void*)((_tmp5AB=
_cycalloc(sizeof(*_tmp5AB)),((_tmp5AB[0]=((_tmp5B0.tag=Cyc_NO_SUPPORT,((_tmp5B0.f1=(
struct _dyneither_ptr)((_tmp5AC.tag=0,((_tmp5AC.f1=(struct _dyneither_ptr)((
_tmp5AD=_tmp1E9,_tag_dyneither(_tmp5AD,sizeof(char),_get_zero_arr_size_char(
_tmp5AD,1)))),((_tmp5AE[0]=& _tmp5AC,Cyc_aprintf(((_tmp5AF="can't open macrosfile %s",
_tag_dyneither(_tmp5AF,sizeof(char),25))),_tag_dyneither(_tmp5AE,sizeof(void*),1)))))))),
_tmp5B0)))),_tmp5AB)))));}in_file=(struct Cyc___cycFILE*)maybe;{struct Cyc_Lexing_lexbuf*
_tmp22F=Cyc_Lexing_from_file(in_file);struct _tuple17*entry;while((entry=((struct
_tuple17*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_line)(_tmp22F))!= 0){struct
_tuple17 _tmp231;struct _dyneither_ptr*_tmp232;struct Cyc_Set_Set*_tmp233;struct
_tuple17*_tmp230=(struct _tuple17*)_check_null(entry);_tmp231=*_tmp230;_tmp232=
_tmp231.f1;_tmp233=_tmp231.f2;((void(*)(struct Cyc_Hashtable_Table*t,struct
_dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp232,
_tmp233);}Cyc_fclose(in_file);maybe=Cyc_fopen(_tmp1EA,(const char*)"r");if(!((
unsigned int)maybe)){struct Cyc_NO_SUPPORT_struct _tmp5C1;const char*_tmp5C0;void*
_tmp5BF[1];const char*_tmp5BE;struct Cyc_String_pa_struct _tmp5BD;struct Cyc_NO_SUPPORT_struct*
_tmp5BC;(int)_throw((void*)((_tmp5BC=_cycalloc(sizeof(*_tmp5BC)),((_tmp5BC[0]=((
_tmp5C1.tag=Cyc_NO_SUPPORT,((_tmp5C1.f1=(struct _dyneither_ptr)((_tmp5BD.tag=0,((
_tmp5BD.f1=(struct _dyneither_ptr)((_tmp5BE=_tmp1EA,_tag_dyneither(_tmp5BE,
sizeof(char),_get_zero_arr_size_char(_tmp5BE,1)))),((_tmp5BF[0]=& _tmp5BD,Cyc_aprintf(((
_tmp5C0="can't open declsfile %s",_tag_dyneither(_tmp5C0,sizeof(char),24))),
_tag_dyneither(_tmp5BF,sizeof(void*),1)))))))),_tmp5C1)))),_tmp5BC)))));}in_file=(
struct Cyc___cycFILE*)maybe;_tmp22F=Cyc_Lexing_from_file(in_file);Cyc_slurp_out=
Cyc_fopen(_tmp1EB,(const char*)"w");if(!((unsigned int)Cyc_slurp_out)){int _tmp23A=
1;_npop_handler(0);return _tmp23A;}while(((int(*)(struct Cyc_Lexing_lexbuf*lexbuf))
Cyc_slurp)(_tmp22F)){;}Cyc_fclose(in_file);Cyc_fclose((struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));if(Cyc_mode != Cyc_FINISH)remove(_tmp1EA);maybe=Cyc_fopen(
_tmp1EB,(const char*)"r");if(!((unsigned int)maybe)){int _tmp23B=1;_npop_handler(0);
return _tmp23B;}in_file=(struct Cyc___cycFILE*)maybe;{const char*_tmp5C2;Cyc_Position_reset_position(((
_tmp5C2=_tmp1EB,_tag_dyneither(_tmp5C2,sizeof(char),_get_zero_arr_size_char(
_tmp5C2,1)))));}Cyc_Lex_lex_init(0);{struct Cyc_List_List*_tmp23D=Cyc_Parse_parse_file(
in_file);Cyc_Lex_lex_init(0);Cyc_fclose(in_file);remove(_tmp1EB);{struct Cyc_List_List*
_tmp23E=_tmp23D;for(0;_tmp23E != 0;_tmp23E=_tmp23E->tl){struct _tuple17*_tmp23F=
Cyc_scan_decl((struct Cyc_Absyn_Decl*)_tmp23E->hd);if(_tmp23F == 0)continue;{
struct _tuple17 _tmp241;struct _dyneither_ptr*_tmp242;struct Cyc_Set_Set*_tmp243;
struct _tuple17*_tmp240=(struct _tuple17*)_tmp23F;_tmp241=*_tmp240;_tmp242=_tmp241.f1;
_tmp243=_tmp241.f2;{struct Cyc_Set_Set*old;{struct _handler_cons _tmp244;
_push_handler(& _tmp244);{int _tmp246=0;if(setjmp(_tmp244.handler))_tmp246=1;if(!
_tmp246){old=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct
_dyneither_ptr*key))Cyc_Hashtable_lookup)(t,_tmp242);;_pop_handler();}else{void*
_tmp245=(void*)_exn_thrown;void*_tmp248=_tmp245;_LL17F: {struct Cyc_Core_Not_found_struct*
_tmp249=(struct Cyc_Core_Not_found_struct*)_tmp248;if(_tmp249->tag != Cyc_Core_Not_found)
goto _LL181;}_LL180: old=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);goto _LL17E;_LL181:;_LL182:(
void)_throw(_tmp248);_LL17E:;}};}((void(*)(struct Cyc_Hashtable_Table*t,struct
_dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp242,((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
_tmp243,old));};};}}{struct Cyc_Set_Set*_tmp24A=Cyc_reachable(start_symbols,t);
struct Cyc_List_List*_tmp24B=0;struct Cyc_Set_Set*defined_symbols=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);{
struct Cyc_List_List*_tmp24C=_tmp23D;for(0;_tmp24C != 0;_tmp24C=_tmp24C->tl){
struct Cyc_Absyn_Decl*_tmp24D=(struct Cyc_Absyn_Decl*)_tmp24C->hd;struct
_dyneither_ptr*name;{void*_tmp24E=_tmp24D->r;struct Cyc_Absyn_Vardecl*_tmp250;
struct Cyc_Absyn_Fndecl*_tmp252;struct Cyc_Absyn_Aggrdecl*_tmp254;struct Cyc_Absyn_Enumdecl*
_tmp256;struct Cyc_Absyn_Typedefdecl*_tmp258;_LL184: {struct Cyc_Absyn_Var_d_struct*
_tmp24F=(struct Cyc_Absyn_Var_d_struct*)_tmp24E;if(_tmp24F->tag != 0)goto _LL186;
else{_tmp250=_tmp24F->f1;}}_LL185: {struct _tuple0 _tmp265;struct _dyneither_ptr*
_tmp266;struct _tuple0*_tmp264=_tmp250->name;_tmp265=*_tmp264;_tmp266=_tmp265.f2;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*
elt))Cyc_Set_insert)(defined_symbols,_tmp266);if(((int(*)(int(*compare)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp266))name=0;else{
name=(struct _dyneither_ptr*)_tmp266;}goto _LL183;}_LL186: {struct Cyc_Absyn_Fn_d_struct*
_tmp251=(struct Cyc_Absyn_Fn_d_struct*)_tmp24E;if(_tmp251->tag != 1)goto _LL188;
else{_tmp252=_tmp251->f1;}}_LL187: {struct _tuple0 _tmp268;struct _dyneither_ptr*
_tmp269;struct _tuple0*_tmp267=_tmp252->name;_tmp268=*_tmp267;_tmp269=_tmp268.f2;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*
elt))Cyc_Set_insert)(defined_symbols,_tmp269);if(((int(*)(int(*compare)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp269))name=0;else{
name=(struct _dyneither_ptr*)_tmp269;}goto _LL183;}_LL188: {struct Cyc_Absyn_Aggr_d_struct*
_tmp253=(struct Cyc_Absyn_Aggr_d_struct*)_tmp24E;if(_tmp253->tag != 6)goto _LL18A;
else{_tmp254=_tmp253->f1;}}_LL189: {struct _tuple0 _tmp26B;struct _dyneither_ptr*
_tmp26C;struct _tuple0*_tmp26A=_tmp254->name;_tmp26B=*_tmp26A;_tmp26C=_tmp26B.f2;
name=(struct _dyneither_ptr*)_tmp26C;goto _LL183;}_LL18A: {struct Cyc_Absyn_Enum_d_struct*
_tmp255=(struct Cyc_Absyn_Enum_d_struct*)_tmp24E;if(_tmp255->tag != 8)goto _LL18C;
else{_tmp256=_tmp255->f1;}}_LL18B: {struct _tuple0 _tmp26E;struct _dyneither_ptr*
_tmp26F;struct _tuple0*_tmp26D=_tmp256->name;_tmp26E=*_tmp26D;_tmp26F=_tmp26E.f2;
name=(struct _dyneither_ptr*)_tmp26F;if(name != 0  && ((int(*)(struct Cyc_Set_Set*s,
struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp24A,(struct _dyneither_ptr*)name)){
struct Cyc_List_List*_tmp5C3;_tmp24B=((_tmp5C3=_cycalloc(sizeof(*_tmp5C3)),((
_tmp5C3->hd=_tmp24D,((_tmp5C3->tl=_tmp24B,_tmp5C3))))));}else{if((unsigned int)
_tmp256->fields){struct Cyc_List_List*_tmp271=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp256->fields))->v;for(0;_tmp271 != 0;_tmp271=_tmp271->tl){struct
Cyc_Absyn_Enumfield*_tmp272=(struct Cyc_Absyn_Enumfield*)_tmp271->hd;struct
_tuple0 _tmp274;struct _dyneither_ptr*_tmp275;struct _tuple0*_tmp273=_tmp272->name;
_tmp274=*_tmp273;_tmp275=_tmp274.f2;if(((int(*)(struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_member)(_tmp24A,_tmp275)){{struct Cyc_List_List*
_tmp5C4;_tmp24B=((_tmp5C4=_cycalloc(sizeof(*_tmp5C4)),((_tmp5C4->hd=_tmp24D,((
_tmp5C4->tl=_tmp24B,_tmp5C4))))));}break;}}}}name=0;goto _LL183;}_LL18C: {struct
Cyc_Absyn_Typedef_d_struct*_tmp257=(struct Cyc_Absyn_Typedef_d_struct*)_tmp24E;
if(_tmp257->tag != 9)goto _LL18E;else{_tmp258=_tmp257->f1;}}_LL18D: {struct _tuple0
_tmp278;struct _dyneither_ptr*_tmp279;struct _tuple0*_tmp277=_tmp258->name;_tmp278=*
_tmp277;_tmp279=_tmp278.f2;name=(struct _dyneither_ptr*)_tmp279;goto _LL183;}
_LL18E: {struct Cyc_Absyn_Porton_d_struct*_tmp259=(struct Cyc_Absyn_Porton_d_struct*)
_tmp24E;if(_tmp259->tag != 14)goto _LL190;}_LL18F: goto _LL191;_LL190: {struct Cyc_Absyn_Portoff_d_struct*
_tmp25A=(struct Cyc_Absyn_Portoff_d_struct*)_tmp24E;if(_tmp25A->tag != 15)goto
_LL192;}_LL191: goto _LL193;_LL192: {struct Cyc_Absyn_Let_d_struct*_tmp25B=(struct
Cyc_Absyn_Let_d_struct*)_tmp24E;if(_tmp25B->tag != 2)goto _LL194;}_LL193: goto
_LL195;_LL194: {struct Cyc_Absyn_Datatype_d_struct*_tmp25C=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp24E;if(_tmp25C->tag != 7)goto _LL196;}_LL195: goto _LL197;_LL196: {struct Cyc_Absyn_Letv_d_struct*
_tmp25D=(struct Cyc_Absyn_Letv_d_struct*)_tmp24E;if(_tmp25D->tag != 3)goto _LL198;}
_LL197: goto _LL199;_LL198: {struct Cyc_Absyn_Namespace_d_struct*_tmp25E=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp24E;if(_tmp25E->tag != 10)goto _LL19A;}_LL199: goto _LL19B;_LL19A: {struct Cyc_Absyn_Using_d_struct*
_tmp25F=(struct Cyc_Absyn_Using_d_struct*)_tmp24E;if(_tmp25F->tag != 11)goto _LL19C;}
_LL19B: goto _LL19D;_LL19C: {struct Cyc_Absyn_ExternC_d_struct*_tmp260=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp24E;if(_tmp260->tag != 12)goto _LL19E;}_LL19D: goto _LL19F;_LL19E: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp261=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp24E;if(_tmp261->tag != 13)
goto _LL1A0;}_LL19F: goto _LL1A1;_LL1A0: {struct Cyc_Absyn_Region_d_struct*_tmp262=(
struct Cyc_Absyn_Region_d_struct*)_tmp24E;if(_tmp262->tag != 4)goto _LL1A2;}_LL1A1:
goto _LL1A3;_LL1A2: {struct Cyc_Absyn_Alias_d_struct*_tmp263=(struct Cyc_Absyn_Alias_d_struct*)
_tmp24E;if(_tmp263->tag != 5)goto _LL183;}_LL1A3: name=0;goto _LL183;_LL183:;}if(
name != 0  && ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
_tmp24A,(struct _dyneither_ptr*)name)){struct Cyc_List_List*_tmp5C5;_tmp24B=((
_tmp5C5=_cycalloc(sizeof(*_tmp5C5)),((_tmp5C5->hd=_tmp24D,((_tmp5C5->tl=_tmp24B,
_tmp5C5))))));}}}if(!Cyc_do_setjmp){maybe=Cyc_fopen(filename,(const char*)"w");
if(!((unsigned int)maybe)){int _tmp27B=1;_npop_handler(0);return _tmp27B;}out_file=(
struct Cyc___cycFILE*)maybe;}else{out_file=Cyc_stdout;}{const char*_tmp5CB;void*
_tmp5CA[1];const char*_tmp5C9;struct Cyc_String_pa_struct _tmp5C8;struct
_dyneither_ptr ifdefmacro=(_tmp5C8.tag=0,((_tmp5C8.f1=(struct _dyneither_ptr)((
_tmp5C9=filename,_tag_dyneither(_tmp5C9,sizeof(char),_get_zero_arr_size_char(
_tmp5C9,1)))),((_tmp5CA[0]=& _tmp5C8,Cyc_aprintf(((_tmp5CB="_%s_",_tag_dyneither(
_tmp5CB,sizeof(char),5))),_tag_dyneither(_tmp5CA,sizeof(void*),1)))))));{int
_tmp27C=0;for(0;_tmp27C < _get_dyneither_size(ifdefmacro,sizeof(char));++ _tmp27C){
if(((char*)ifdefmacro.curr)[_tmp27C]== '.'  || ((char*)ifdefmacro.curr)[_tmp27C]
== '/'){char _tmp5CE;char _tmp5CD;struct _dyneither_ptr _tmp5CC;(_tmp5CC=
_dyneither_ptr_plus(ifdefmacro,sizeof(char),_tmp27C),((_tmp5CD=*((char*)
_check_dyneither_subscript(_tmp5CC,sizeof(char),0)),((_tmp5CE='_',((
_get_dyneither_size(_tmp5CC,sizeof(char))== 1  && (_tmp5CD == '\000'  && _tmp5CE != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp5CC.curr)=_tmp5CE)))))));}else{if(((char*)
ifdefmacro.curr)[_tmp27C]!= '_'  && ((char*)ifdefmacro.curr)[_tmp27C]!= '/'){char
_tmp5D1;char _tmp5D0;struct _dyneither_ptr _tmp5CF;(_tmp5CF=_dyneither_ptr_plus(
ifdefmacro,sizeof(char),_tmp27C),((_tmp5D0=*((char*)_check_dyneither_subscript(
_tmp5CF,sizeof(char),0)),((_tmp5D1=(char)toupper((int)((char*)ifdefmacro.curr)[
_tmp27C]),((_get_dyneither_size(_tmp5CF,sizeof(char))== 1  && (_tmp5D0 == '\000'
 && _tmp5D1 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5CF.curr)=_tmp5D1)))))));}}}}{
const char*_tmp5D6;void*_tmp5D5[2];struct Cyc_String_pa_struct _tmp5D4;struct Cyc_String_pa_struct
_tmp5D3;(_tmp5D3.tag=0,((_tmp5D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
ifdefmacro),((_tmp5D4.tag=0,((_tmp5D4.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)ifdefmacro),((_tmp5D5[0]=& _tmp5D4,((_tmp5D5[1]=& _tmp5D3,Cyc_fprintf(
out_file,((_tmp5D6="#ifndef %s\n#define %s\n",_tag_dyneither(_tmp5D6,sizeof(char),
23))),_tag_dyneither(_tmp5D5,sizeof(void*),2)))))))))))));}{struct Cyc_List_List*
_tmp287=0;struct Cyc_List_List*_tmp288=0;{struct Cyc_List_List*_tmp289=_tmp24B;
for(0;_tmp289 != 0;_tmp289=_tmp289->tl){struct Cyc_Absyn_Decl*_tmp28A=(struct Cyc_Absyn_Decl*)
_tmp289->hd;int _tmp28B=0;struct _dyneither_ptr*name;{void*_tmp28C=_tmp28A->r;
struct Cyc_Absyn_Vardecl*_tmp28E;struct Cyc_Absyn_Fndecl*_tmp290;struct Cyc_Absyn_Aggrdecl*
_tmp292;struct Cyc_Absyn_Enumdecl*_tmp294;struct Cyc_Absyn_Typedefdecl*_tmp296;
_LL1A5: {struct Cyc_Absyn_Var_d_struct*_tmp28D=(struct Cyc_Absyn_Var_d_struct*)
_tmp28C;if(_tmp28D->tag != 0)goto _LL1A7;else{_tmp28E=_tmp28D->f1;}}_LL1A6: {
struct _tuple0 _tmp2A3;struct _dyneither_ptr*_tmp2A4;struct _tuple0*_tmp2A2=_tmp28E->name;
_tmp2A3=*_tmp2A2;_tmp2A4=_tmp2A3.f2;name=(struct _dyneither_ptr*)_tmp2A4;goto
_LL1A4;}_LL1A7: {struct Cyc_Absyn_Fn_d_struct*_tmp28F=(struct Cyc_Absyn_Fn_d_struct*)
_tmp28C;if(_tmp28F->tag != 1)goto _LL1A9;else{_tmp290=_tmp28F->f1;}}_LL1A8: {
struct _tuple0 _tmp2A6;struct _dyneither_ptr*_tmp2A7;struct _tuple0*_tmp2A5=_tmp290->name;
_tmp2A6=*_tmp2A5;_tmp2A7=_tmp2A6.f2;name=(struct _dyneither_ptr*)_tmp2A7;goto
_LL1A4;}_LL1A9: {struct Cyc_Absyn_Aggr_d_struct*_tmp291=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp28C;if(_tmp291->tag != 6)goto _LL1AB;else{_tmp292=_tmp291->f1;}}_LL1AA: {
struct _tuple0 _tmp2A9;struct _dyneither_ptr*_tmp2AA;struct _tuple0*_tmp2A8=_tmp292->name;
_tmp2A9=*_tmp2A8;_tmp2AA=_tmp2A9.f2;name=(struct _dyneither_ptr*)_tmp2AA;goto
_LL1A4;}_LL1AB: {struct Cyc_Absyn_Enum_d_struct*_tmp293=(struct Cyc_Absyn_Enum_d_struct*)
_tmp28C;if(_tmp293->tag != 8)goto _LL1AD;else{_tmp294=_tmp293->f1;}}_LL1AC: {
struct _tuple0 _tmp2AC;struct _dyneither_ptr*_tmp2AD;struct _tuple0*_tmp2AB=_tmp294->name;
_tmp2AC=*_tmp2AB;_tmp2AD=_tmp2AC.f2;name=(struct _dyneither_ptr*)_tmp2AD;goto
_LL1A4;}_LL1AD: {struct Cyc_Absyn_Typedef_d_struct*_tmp295=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp28C;if(_tmp295->tag != 9)goto _LL1AF;else{_tmp296=_tmp295->f1;}}_LL1AE: {
struct _tuple0 _tmp2AF;struct _dyneither_ptr*_tmp2B0;struct _tuple0*_tmp2AE=_tmp296->name;
_tmp2AF=*_tmp2AE;_tmp2B0=_tmp2AF.f2;name=(struct _dyneither_ptr*)_tmp2B0;goto
_LL1A4;}_LL1AF: {struct Cyc_Absyn_Region_d_struct*_tmp297=(struct Cyc_Absyn_Region_d_struct*)
_tmp28C;if(_tmp297->tag != 4)goto _LL1B1;}_LL1B0: goto _LL1B2;_LL1B1: {struct Cyc_Absyn_Alias_d_struct*
_tmp298=(struct Cyc_Absyn_Alias_d_struct*)_tmp28C;if(_tmp298->tag != 5)goto _LL1B3;}
_LL1B2: goto _LL1B4;_LL1B3: {struct Cyc_Absyn_Porton_d_struct*_tmp299=(struct Cyc_Absyn_Porton_d_struct*)
_tmp28C;if(_tmp299->tag != 14)goto _LL1B5;}_LL1B4: goto _LL1B6;_LL1B5: {struct Cyc_Absyn_Portoff_d_struct*
_tmp29A=(struct Cyc_Absyn_Portoff_d_struct*)_tmp28C;if(_tmp29A->tag != 15)goto
_LL1B7;}_LL1B6: goto _LL1B8;_LL1B7: {struct Cyc_Absyn_Let_d_struct*_tmp29B=(struct
Cyc_Absyn_Let_d_struct*)_tmp28C;if(_tmp29B->tag != 2)goto _LL1B9;}_LL1B8: goto
_LL1BA;_LL1B9: {struct Cyc_Absyn_Datatype_d_struct*_tmp29C=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp28C;if(_tmp29C->tag != 7)goto _LL1BB;}_LL1BA: goto _LL1BC;_LL1BB: {struct Cyc_Absyn_Letv_d_struct*
_tmp29D=(struct Cyc_Absyn_Letv_d_struct*)_tmp28C;if(_tmp29D->tag != 3)goto _LL1BD;}
_LL1BC: goto _LL1BE;_LL1BD: {struct Cyc_Absyn_Namespace_d_struct*_tmp29E=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp28C;if(_tmp29E->tag != 10)goto _LL1BF;}_LL1BE: goto _LL1C0;_LL1BF: {struct Cyc_Absyn_Using_d_struct*
_tmp29F=(struct Cyc_Absyn_Using_d_struct*)_tmp28C;if(_tmp29F->tag != 11)goto _LL1C1;}
_LL1C0: goto _LL1C2;_LL1C1: {struct Cyc_Absyn_ExternC_d_struct*_tmp2A0=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp28C;if(_tmp2A0->tag != 12)goto _LL1C3;}_LL1C2: goto _LL1C4;_LL1C3: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp2A1=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp28C;if(_tmp2A1->tag != 13)
goto _LL1A4;}_LL1C4: name=0;goto _LL1A4;_LL1A4:;}if(!((unsigned int)name) && !
_tmp28B)continue;{struct Cyc_List_List*_tmp5D7;_tmp287=((_tmp5D7=_cycalloc(
sizeof(*_tmp5D7)),((_tmp5D7->hd=_tmp28A,((_tmp5D7->tl=_tmp287,_tmp5D7))))));}{
struct Cyc_List_List*_tmp5D8;_tmp288=((_tmp5D8=_cycalloc(sizeof(*_tmp5D8)),((
_tmp5D8->hd=name,((_tmp5D8->tl=_tmp288,_tmp5D8))))));};}}{struct _RegionHandle
_tmp2B3=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmp2B3;_push_region(
tc_rgn);{struct Cyc_Tcenv_Tenv*_tmp2B4=Cyc_Tcenv_tc_init(tc_rgn);Cyc_Tc_tc(tc_rgn,
_tmp2B4,1,_tmp287);}{struct Cyc_List_List*_tmp2B7;struct Cyc_List_List*_tmp2B8;
struct _tuple23 _tmp5D9;struct _tuple23 _tmp2B6=(_tmp5D9.f1=_tmp287,((_tmp5D9.f2=
_tmp288,_tmp5D9)));_tmp2B7=_tmp2B6.f1;_tmp2B8=_tmp2B6.f2;for(0;_tmp2B7 != 0  && 
_tmp2B8 != 0;(_tmp2B7=_tmp2B7->tl,_tmp2B8=_tmp2B8->tl)){struct Cyc_Absyn_Decl*
_tmp2B9=(struct Cyc_Absyn_Decl*)_tmp2B7->hd;struct _dyneither_ptr*_tmp2BA=(struct
_dyneither_ptr*)_tmp2B8->hd;int _tmp2BB=0;if(!((unsigned int)_tmp2BA))_tmp2BB=1;
if((unsigned int)_tmp2BA){{const char*_tmp5DD;void*_tmp5DC[1];struct Cyc_String_pa_struct
_tmp5DB;ifdefmacro=((_tmp5DB.tag=0,((_tmp5DB.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp2BA),((_tmp5DC[0]=& _tmp5DB,Cyc_aprintf(((_tmp5DD="_%s_def_",
_tag_dyneither(_tmp5DD,sizeof(char),9))),_tag_dyneither(_tmp5DC,sizeof(void*),1))))))));}{
const char*_tmp5E1;void*_tmp5E0[1];struct Cyc_String_pa_struct _tmp5DF;(_tmp5DF.tag=
0,((_tmp5DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro),((
_tmp5E0[0]=& _tmp5DF,Cyc_fprintf(out_file,((_tmp5E1="#ifndef %s\n",_tag_dyneither(
_tmp5E1,sizeof(char),12))),_tag_dyneither(_tmp5E0,sizeof(void*),1)))))));}{const
char*_tmp5E5;void*_tmp5E4[1];struct Cyc_String_pa_struct _tmp5E3;(_tmp5E3.tag=0,((
_tmp5E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro),((_tmp5E4[0]=&
_tmp5E3,Cyc_fprintf(out_file,((_tmp5E5="#define %s\n",_tag_dyneither(_tmp5E5,
sizeof(char),12))),_tag_dyneither(_tmp5E4,sizeof(void*),1)))))));}{struct Cyc_Absyn_Decl*
_tmp5E6[1];Cyc_Absynpp_decllist2file(((_tmp5E6[0]=_tmp2B9,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5E6,sizeof(struct Cyc_Absyn_Decl*),
1)))),out_file);}{const char*_tmp5E9;void*_tmp5E8;(_tmp5E8=0,Cyc_fprintf(out_file,((
_tmp5E9="#endif\n",_tag_dyneither(_tmp5E9,sizeof(char),8))),_tag_dyneither(
_tmp5E8,sizeof(void*),0)));};}else{struct Cyc_Absyn_Decl*_tmp5EA[1];Cyc_Absynpp_decllist2file(((
_tmp5EA[0]=_tmp2B9,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp5EA,sizeof(struct Cyc_Absyn_Decl*),1)))),out_file);}}}maybe=
Cyc_fopen(_tmp1E9,(const char*)"r");if(!((unsigned int)maybe)){struct Cyc_NO_SUPPORT_struct
_tmp5FB;const char*_tmp5FA;void*_tmp5F9[1];const char*_tmp5F8;struct Cyc_String_pa_struct
_tmp5F7;struct Cyc_NO_SUPPORT_struct*_tmp5F6;(int)_throw((void*)((_tmp5F6=
_cycalloc(sizeof(*_tmp5F6)),((_tmp5F6[0]=((_tmp5FB.tag=Cyc_NO_SUPPORT,((_tmp5FB.f1=(
struct _dyneither_ptr)((_tmp5F7.tag=0,((_tmp5F7.f1=(struct _dyneither_ptr)((
_tmp5F8=_tmp1E9,_tag_dyneither(_tmp5F8,sizeof(char),_get_zero_arr_size_char(
_tmp5F8,1)))),((_tmp5F9[0]=& _tmp5F7,Cyc_aprintf(((_tmp5FA="can't open macrosfile %s",
_tag_dyneither(_tmp5FA,sizeof(char),25))),_tag_dyneither(_tmp5F9,sizeof(void*),1)))))))),
_tmp5FB)))),_tmp5F6)))));}in_file=(struct Cyc___cycFILE*)maybe;_tmp22F=Cyc_Lexing_from_file(
in_file);{struct _tuple18*entry2;while((entry2=((struct _tuple18*(*)(struct Cyc_Lexing_lexbuf*
lexbuf))Cyc_suck_line)(_tmp22F))!= 0){struct _tuple18 _tmp2D0;struct _dyneither_ptr
_tmp2D1;struct _dyneither_ptr*_tmp2D2;struct _tuple18*_tmp2CF=(struct _tuple18*)
_check_null(entry2);_tmp2D0=*_tmp2CF;_tmp2D1=_tmp2D0.f1;_tmp2D2=_tmp2D0.f2;if(((
int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp24A,
_tmp2D2)){{const char*_tmp5FF;void*_tmp5FE[1];struct Cyc_String_pa_struct _tmp5FD;(
_tmp5FD.tag=0,((_tmp5FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2D2),((
_tmp5FE[0]=& _tmp5FD,Cyc_fprintf(out_file,((_tmp5FF="#ifndef %s\n",_tag_dyneither(
_tmp5FF,sizeof(char),12))),_tag_dyneither(_tmp5FE,sizeof(void*),1)))))));}{const
char*_tmp603;void*_tmp602[1];struct Cyc_String_pa_struct _tmp601;(_tmp601.tag=0,((
_tmp601.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2D1),((_tmp602[0]=&
_tmp601,Cyc_fprintf(out_file,((_tmp603="%s\n",_tag_dyneither(_tmp603,sizeof(char),
4))),_tag_dyneither(_tmp602,sizeof(void*),1)))))));}{const char*_tmp606;void*
_tmp605;(_tmp605=0,Cyc_fprintf(out_file,((_tmp606="#endif\n",_tag_dyneither(
_tmp606,sizeof(char),8))),_tag_dyneither(_tmp605,sizeof(void*),0)));};}}Cyc_fclose(
in_file);if(Cyc_mode != Cyc_FINISH)remove(_tmp1E9);if(hstubs != 0){struct Cyc_List_List*
_tmp2DB=hstubs;for(0;_tmp2DB != 0;_tmp2DB=_tmp2DB->tl){struct _tuple19 _tmp2DD;
struct _dyneither_ptr _tmp2DE;struct _dyneither_ptr _tmp2DF;struct _tuple19*_tmp2DC=(
struct _tuple19*)_tmp2DB->hd;_tmp2DD=*_tmp2DC;_tmp2DE=_tmp2DD.f1;_tmp2DF=_tmp2DD.f2;{
struct _dyneither_ptr*_tmp607;if(_tmp2DF.curr != (_tag_dyneither(0,0,0)).curr  && (
_tmp2DE.curr == (_tag_dyneither(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*s,
struct _dyneither_ptr*elt))Cyc_Set_member)(defined_symbols,((_tmp607=_cycalloc(
sizeof(*_tmp607)),((_tmp607[0]=_tmp2DE,_tmp607)))))))Cyc_fputs((const char*)
_untag_dyneither_ptr(_tmp2DF,sizeof(char),1),out_file);else{const char*_tmp60B;
void*_tmp60A[1];struct Cyc_String_pa_struct _tmp609;(_tmp609.tag=0,((_tmp609.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2DE),((_tmp60A[0]=& _tmp609,Cyc_log(((
_tmp60B="%s is not supported on this platform\n",_tag_dyneither(_tmp60B,sizeof(
char),38))),_tag_dyneither(_tmp60A,sizeof(void*),1)))))));}};}}{const char*
_tmp60E;void*_tmp60D;(_tmp60D=0,Cyc_fprintf(out_file,((_tmp60E="#endif\n",
_tag_dyneither(_tmp60E,sizeof(char),8))),_tag_dyneither(_tmp60D,sizeof(void*),0)));}
if(Cyc_do_setjmp){int _tmp2E6=0;_npop_handler(1);return _tmp2E6;}else{Cyc_fclose(
out_file);}if(cstubs != 0){out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file);{
struct Cyc_List_List*_tmp2E7=cstubs;for(0;_tmp2E7 != 0;_tmp2E7=_tmp2E7->tl){struct
_tuple19 _tmp2E9;struct _dyneither_ptr _tmp2EA;struct _dyneither_ptr _tmp2EB;struct
_tuple19*_tmp2E8=(struct _tuple19*)_tmp2E7->hd;_tmp2E9=*_tmp2E8;_tmp2EA=_tmp2E9.f1;
_tmp2EB=_tmp2E9.f2;{struct _dyneither_ptr*_tmp60F;if(_tmp2EB.curr != (
_tag_dyneither(0,0,0)).curr  && (_tmp2EA.curr == (_tag_dyneither(0,0,0)).curr  || ((
int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
defined_symbols,((_tmp60F=_cycalloc(sizeof(*_tmp60F)),((_tmp60F[0]=_tmp2EA,
_tmp60F)))))))Cyc_fputs((const char*)_untag_dyneither_ptr(_tmp2EB,sizeof(char),1),
out_file);};}};}out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);{
const char*_tmp615;void*_tmp614[1];const char*_tmp613;struct Cyc_String_pa_struct
_tmp612;(_tmp612.tag=0,((_tmp612.f1=(struct _dyneither_ptr)((_tmp613=filename,
_tag_dyneither(_tmp613,sizeof(char),_get_zero_arr_size_char(_tmp613,1)))),((
_tmp614[0]=& _tmp612,Cyc_fprintf(out_file,((_tmp615="#include <%s>\n\n",
_tag_dyneither(_tmp615,sizeof(char),16))),_tag_dyneither(_tmp614,sizeof(void*),1)))))));}
if(cycstubs != 0){out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);{
struct Cyc_List_List*_tmp2F1=cycstubs;for(0;_tmp2F1 != 0;_tmp2F1=_tmp2F1->tl){
struct _tuple19 _tmp2F3;struct _dyneither_ptr _tmp2F4;struct _dyneither_ptr _tmp2F5;
struct _tuple19*_tmp2F2=(struct _tuple19*)_tmp2F1->hd;_tmp2F3=*_tmp2F2;_tmp2F4=
_tmp2F3.f1;_tmp2F5=_tmp2F3.f2;{struct _dyneither_ptr*_tmp616;if(_tmp2F5.curr != (
_tag_dyneither(0,0,0)).curr  && (_tmp2F4.curr == (_tag_dyneither(0,0,0)).curr  || ((
int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
defined_symbols,((_tmp616=_cycalloc(sizeof(*_tmp616)),((_tmp616[0]=_tmp2F4,
_tmp616)))))))Cyc_fputs((const char*)_untag_dyneither_ptr(_tmp2F5,sizeof(char),1),
out_file);};}}{const char*_tmp619;void*_tmp618;(_tmp618=0,Cyc_fprintf(out_file,((
_tmp619="\n",_tag_dyneither(_tmp619,sizeof(char),2))),_tag_dyneither(_tmp618,
sizeof(void*),0)));};}{int _tmp2F9=0;_npop_handler(1);return _tmp2F9;};};;
_pop_region(tc_rgn);};};};};};};};;_pop_handler();}else{void*_tmp1ED=(void*)
_exn_thrown;void*_tmp2FF=_tmp1ED;struct _dyneither_ptr _tmp301;_LL1C6: {struct Cyc_NO_SUPPORT_struct*
_tmp300=(struct Cyc_NO_SUPPORT_struct*)_tmp2FF;if(_tmp300->tag != Cyc_NO_SUPPORT)
goto _LL1C8;else{_tmp301=_tmp300->f1;}}_LL1C7:{const char*_tmp61D;void*_tmp61C[1];
struct Cyc_String_pa_struct _tmp61B;(_tmp61B.tag=0,((_tmp61B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp301),((_tmp61C[0]=& _tmp61B,Cyc_fprintf(
Cyc_stderr,((_tmp61D="No support because %s\n",_tag_dyneither(_tmp61D,sizeof(
char),23))),_tag_dyneither(_tmp61C,sizeof(void*),1)))))));}goto _LL1C9;_LL1C8:;
_LL1C9: maybe=Cyc_fopen(filename,(const char*)"w");if(!((unsigned int)maybe)){{
const char*_tmp623;void*_tmp622[1];const char*_tmp621;struct Cyc_String_pa_struct
_tmp620;(_tmp620.tag=0,((_tmp620.f1=(struct _dyneither_ptr)((_tmp621=filename,
_tag_dyneither(_tmp621,sizeof(char),_get_zero_arr_size_char(_tmp621,1)))),((
_tmp622[0]=& _tmp620,Cyc_fprintf(Cyc_stderr,((_tmp623="Error: could not create file %s\n",
_tag_dyneither(_tmp623,sizeof(char),33))),_tag_dyneither(_tmp622,sizeof(void*),1)))))));}
return 1;}out_file=(struct Cyc___cycFILE*)maybe;{const char*_tmp629;void*_tmp628[1];
const char*_tmp627;struct Cyc_String_pa_struct _tmp626;(_tmp626.tag=0,((_tmp626.f1=(
struct _dyneither_ptr)((_tmp627=filename,_tag_dyneither(_tmp627,sizeof(char),
_get_zero_arr_size_char(_tmp627,1)))),((_tmp628[0]=& _tmp626,Cyc_fprintf(out_file,((
_tmp629="#error -- %s is not supported on this platform\n",_tag_dyneither(
_tmp629,sizeof(char),48))),_tag_dyneither(_tmp628,sizeof(void*),1)))))));}Cyc_fclose(
out_file);{const char*_tmp62F;void*_tmp62E[1];const char*_tmp62D;struct Cyc_String_pa_struct
_tmp62C;(_tmp62C.tag=0,((_tmp62C.f1=(struct _dyneither_ptr)((_tmp62D=filename,
_tag_dyneither(_tmp62D,sizeof(char),_get_zero_arr_size_char(_tmp62D,1)))),((
_tmp62E[0]=& _tmp62C,Cyc_fprintf(Cyc_stderr,((_tmp62F="Warning: %s will not be supported on this platform\n",
_tag_dyneither(_tmp62F,sizeof(char),52))),_tag_dyneither(_tmp62E,sizeof(void*),1)))))));}{
const char*_tmp632;void*_tmp631;(_tmp631=0,Cyc_log(((_tmp632="Not supported on this platform\n",
_tag_dyneither(_tmp632,sizeof(char),32))),_tag_dyneither(_tmp631,sizeof(void*),0)));}
remove(_tmp1E9);remove(_tmp1EA);remove(_tmp1EB);return 0;_LL1CA:;_LL1CB:(void)
_throw(_tmp2FF);_LL1C5:;}};};}int Cyc_process_specfile(const char*file,const char*
dir);static void _tmp63D(unsigned int*_tmp63C,unsigned int*_tmp63B,char**_tmp63A){
for(*_tmp63C=0;*_tmp63C < *_tmp63B;(*_tmp63C)++){(*_tmp63A)[*_tmp63C]='\000';}}
int Cyc_process_specfile(const char*file,const char*dir){struct Cyc___cycFILE*
_tmp328=Cyc_fopen(file,(const char*)"r");if(!((unsigned int)_tmp328)){{const char*
_tmp638;void*_tmp637[1];const char*_tmp636;struct Cyc_String_pa_struct _tmp635;(
_tmp635.tag=0,((_tmp635.f1=(struct _dyneither_ptr)((_tmp636=file,_tag_dyneither(
_tmp636,sizeof(char),_get_zero_arr_size_char(_tmp636,1)))),((_tmp637[0]=& _tmp635,
Cyc_fprintf(Cyc_stderr,((_tmp638="Error: could not open %s\n",_tag_dyneither(
_tmp638,sizeof(char),26))),_tag_dyneither(_tmp637,sizeof(void*),1)))))));}return
1;}{struct Cyc___cycFILE*_tmp32D=(struct Cyc___cycFILE*)_tmp328;char*_tmp642;
unsigned int _tmp641;char*_tmp640;unsigned int _tmp63F;unsigned int _tmp63E;struct
_dyneither_ptr buf=(_tmp642=((_tmp641=(unsigned int)1024,((_tmp640=(char*)
_cycalloc_atomic(_check_times(sizeof(char),_tmp641 + 1)),((((_tmp63F=_tmp641,((
_tmp63D(& _tmp63E,& _tmp63F,& _tmp640),_tmp640[_tmp63F]=(char)0)))),_tmp640)))))),
_tag_dyneither(_tmp642,sizeof(char),_get_zero_arr_size_char(_tmp642,(
unsigned int)1024 + 1)));struct _dyneither_ptr _tmp32E=Cyc_getcwd(buf,
_get_dyneither_size(buf,sizeof(char)));if(Cyc_mode != Cyc_GATHERSCRIPT){if(chdir(
dir)){{const char*_tmp648;void*_tmp647[1];const char*_tmp646;struct Cyc_String_pa_struct
_tmp645;(_tmp645.tag=0,((_tmp645.f1=(struct _dyneither_ptr)((_tmp646=dir,
_tag_dyneither(_tmp646,sizeof(char),_get_zero_arr_size_char(_tmp646,1)))),((
_tmp647[0]=& _tmp645,Cyc_fprintf(Cyc_stderr,((_tmp648="Error: can't change directory to %s\n",
_tag_dyneither(_tmp648,sizeof(char),37))),_tag_dyneither(_tmp647,sizeof(void*),1)))))));}
return 1;}}if(Cyc_mode == Cyc_GATHER){struct _dyneither_ptr _tmp333=
Cstring_to_string(Ccomp);const char*_tmp64C;void*_tmp64B[1];struct Cyc_String_pa_struct
_tmp64A;system((const char*)_untag_dyneither_ptr(((_tmp64A.tag=0,((_tmp64A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp333),((_tmp64B[0]=& _tmp64A,Cyc_aprintf(((
_tmp64C="echo | %s -E -dM - -o INITMACROS.h\n",_tag_dyneither(_tmp64C,sizeof(
char),36))),_tag_dyneither(_tmp64B,sizeof(void*),1)))))))),sizeof(char),1));}{
struct Cyc_Lexing_lexbuf*_tmp337=Cyc_Lexing_from_file(_tmp32D);struct _tuple20*
entry;while((entry=((struct _tuple20*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_spec)(
_tmp337))!= 0){struct _tuple20 _tmp339;struct _dyneither_ptr _tmp33A;struct Cyc_List_List*
_tmp33B;struct Cyc_List_List*_tmp33C;struct Cyc_List_List*_tmp33D;struct Cyc_List_List*
_tmp33E;struct Cyc_List_List*_tmp33F;struct _tuple20*_tmp338=(struct _tuple20*)
_check_null(entry);_tmp339=*_tmp338;_tmp33A=_tmp339.f1;_tmp33B=_tmp339.f2;
_tmp33C=_tmp339.f3;_tmp33D=_tmp339.f4;_tmp33E=_tmp339.f5;_tmp33F=_tmp339.f6;if(
Cyc_process_file((const char*)_untag_dyneither_ptr(_tmp33A,sizeof(char),1),
_tmp33B,_tmp33C,_tmp33D,_tmp33E,_tmp33F))return 1;}Cyc_fclose(_tmp32D);if(Cyc_mode
!= Cyc_GATHERSCRIPT){if(chdir((const char*)((char*)_untag_dyneither_ptr(_tmp32E,
sizeof(char),1)))){{const char*_tmp650;void*_tmp64F[1];struct Cyc_String_pa_struct
_tmp64E;(_tmp64E.tag=0,((_tmp64E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp32E),((_tmp64F[0]=& _tmp64E,Cyc_fprintf(Cyc_stderr,((_tmp650="Error: could not change directory to %s\n",
_tag_dyneither(_tmp650,sizeof(char),41))),_tag_dyneither(_tmp64F,sizeof(void*),1)))))));}
return 1;}}return 0;};};}int Cyc_process_setjmp(const char*dir);static void _tmp655(
unsigned int*_tmp654,unsigned int*_tmp653,char**_tmp652){for(*_tmp654=0;*_tmp654
< *_tmp653;(*_tmp654)++){(*_tmp652)[*_tmp654]='\000';}}int Cyc_process_setjmp(
const char*dir){char*_tmp65A;unsigned int _tmp659;char*_tmp658;unsigned int _tmp657;
unsigned int _tmp656;struct _dyneither_ptr buf=(_tmp65A=((_tmp659=(unsigned int)
1024,((_tmp658=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp659 + 1)),((((
_tmp657=_tmp659,((_tmp655(& _tmp656,& _tmp657,& _tmp658),_tmp658[_tmp657]=(char)0)))),
_tmp658)))))),_tag_dyneither(_tmp65A,sizeof(char),_get_zero_arr_size_char(
_tmp65A,(unsigned int)1024 + 1)));struct _dyneither_ptr _tmp347=Cyc_getcwd(buf,
_get_dyneither_size(buf,sizeof(char)));if(chdir(dir)){{const char*_tmp660;void*
_tmp65F[1];const char*_tmp65E;struct Cyc_String_pa_struct _tmp65D;(_tmp65D.tag=0,((
_tmp65D.f1=(struct _dyneither_ptr)((_tmp65E=dir,_tag_dyneither(_tmp65E,sizeof(
char),_get_zero_arr_size_char(_tmp65E,1)))),((_tmp65F[0]=& _tmp65D,Cyc_fprintf(
Cyc_stderr,((_tmp660="Error: can't change directory to %s\n",_tag_dyneither(
_tmp660,sizeof(char),37))),_tag_dyneither(_tmp65F,sizeof(void*),1)))))));}return
1;}{struct _tuple19*_tmp66A;const char*_tmp669;const char*_tmp668;struct _tuple19*
_tmp667[1];struct _dyneither_ptr*_tmp661[1];if(Cyc_process_file((const char*)"setjmp.h",((
_tmp661[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"jmp_buf",
sizeof(char),8),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp661,sizeof(struct _dyneither_ptr*),1)))),0,((_tmp667[0]=((
_tmp66A=_cycalloc(sizeof(*_tmp66A)),((_tmp66A->f1=((_tmp669="setjmp",
_tag_dyneither(_tmp669,sizeof(char),7))),((_tmp66A->f2=((_tmp668="extern int setjmp(jmp_buf);\n",
_tag_dyneither(_tmp668,sizeof(char),29))),_tmp66A)))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp667,sizeof(struct
_tuple19*),1)))),0,0))return 1;}if(chdir((const char*)((char*)_untag_dyneither_ptr(
_tmp347,sizeof(char),1)))){{const char*_tmp66E;void*_tmp66D[1];struct Cyc_String_pa_struct
_tmp66C;(_tmp66C.tag=0,((_tmp66C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp347),((_tmp66D[0]=& _tmp66C,Cyc_fprintf(Cyc_stderr,((_tmp66E="Error: could not change directory to %s\n",
_tag_dyneither(_tmp66E,sizeof(char),41))),_tag_dyneither(_tmp66D,sizeof(void*),1)))))));}
return 1;}return 0;}static char _tmp359[13]="BUILDLIB.OUT";static struct
_dyneither_ptr Cyc_output_dir={_tmp359,_tmp359,_tmp359 + 13};static void Cyc_set_output_dir(
struct _dyneither_ptr s);static void Cyc_set_output_dir(struct _dyneither_ptr s){Cyc_output_dir=
s;}static struct Cyc_List_List*Cyc_spec_files=0;static void Cyc_add_spec_file(struct
_dyneither_ptr s);static void Cyc_add_spec_file(struct _dyneither_ptr s){struct Cyc_List_List*
_tmp66F;Cyc_spec_files=((_tmp66F=_cycalloc(sizeof(*_tmp66F)),((_tmp66F->hd=(
const char*)_untag_dyneither_ptr(s,sizeof(char),1),((_tmp66F->tl=Cyc_spec_files,
_tmp66F))))));}static void Cyc_set_GATHER();static void Cyc_set_GATHER(){Cyc_mode=
Cyc_GATHER;}static void Cyc_set_GATHERSCRIPT();static void Cyc_set_GATHERSCRIPT(){
Cyc_mode=Cyc_GATHERSCRIPT;}static void Cyc_set_FINISH();static void Cyc_set_FINISH(){
Cyc_mode=Cyc_FINISH;}static int Cyc_badparse=0;static void Cyc_unsupported_option(
struct _dyneither_ptr s);static void Cyc_unsupported_option(struct _dyneither_ptr s){{
const char*_tmp673;void*_tmp672[1];struct Cyc_String_pa_struct _tmp671;(_tmp671.tag=
0,((_tmp671.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp672[0]=&
_tmp671,Cyc_fprintf(Cyc_stderr,((_tmp673="Unsupported option %s\n",
_tag_dyneither(_tmp673,sizeof(char),23))),_tag_dyneither(_tmp672,sizeof(void*),1)))))));}
Cyc_badparse=1;}void GC_blacklist_warn_clear();struct _tuple24{struct
_dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
int Cyc_main(int argc,struct _dyneither_ptr argv);int Cyc_main(int argc,struct
_dyneither_ptr argv){GC_blacklist_warn_clear();{struct _tuple24*_tmp6E0;const char*
_tmp6DF;const char*_tmp6DE;struct Cyc_Arg_String_spec_struct _tmp6DD;struct Cyc_Arg_String_spec_struct*
_tmp6DC;const char*_tmp6DB;struct _tuple24*_tmp6DA;const char*_tmp6D9;const char*
_tmp6D8;struct Cyc_Arg_Unit_spec_struct _tmp6D7;struct Cyc_Arg_Unit_spec_struct*
_tmp6D6;const char*_tmp6D5;struct _tuple24*_tmp6D4;const char*_tmp6D3;const char*
_tmp6D2;struct Cyc_Arg_Unit_spec_struct _tmp6D1;struct Cyc_Arg_Unit_spec_struct*
_tmp6D0;const char*_tmp6CF;struct _tuple24*_tmp6CE;const char*_tmp6CD;const char*
_tmp6CC;struct Cyc_Arg_Unit_spec_struct _tmp6CB;struct Cyc_Arg_Unit_spec_struct*
_tmp6CA;const char*_tmp6C9;struct _tuple24*_tmp6C8;const char*_tmp6C7;const char*
_tmp6C6;struct Cyc_Arg_Set_spec_struct _tmp6C5;struct Cyc_Arg_Set_spec_struct*
_tmp6C4;const char*_tmp6C3;struct _tuple24*_tmp6C2;const char*_tmp6C1;const char*
_tmp6C0;struct Cyc_Arg_Flag_spec_struct _tmp6BF;struct Cyc_Arg_Flag_spec_struct*
_tmp6BE;const char*_tmp6BD;struct _tuple24*_tmp6BC[6];struct Cyc_List_List*options=(
_tmp6BC[5]=((_tmp6C2=_cycalloc(sizeof(*_tmp6C2)),((_tmp6C2->f1=((_tmp6C1="-",
_tag_dyneither(_tmp6C1,sizeof(char),2))),((_tmp6C2->f2=1,((_tmp6C2->f3=((_tmp6C0="",
_tag_dyneither(_tmp6C0,sizeof(char),1))),((_tmp6C2->f4=(void*)((_tmp6BE=
_cycalloc(sizeof(*_tmp6BE)),((_tmp6BE[0]=((_tmp6BF.tag=1,((_tmp6BF.f1=Cyc_unsupported_option,
_tmp6BF)))),_tmp6BE)))),((_tmp6C2->f5=((_tmp6BD="",_tag_dyneither(_tmp6BD,
sizeof(char),1))),_tmp6C2)))))))))))),((_tmp6BC[4]=((_tmp6C8=_cycalloc(sizeof(*
_tmp6C8)),((_tmp6C8->f1=((_tmp6C7="-setjmp",_tag_dyneither(_tmp6C7,sizeof(char),
8))),((_tmp6C8->f2=0,((_tmp6C8->f3=((_tmp6C6="",_tag_dyneither(_tmp6C6,sizeof(
char),1))),((_tmp6C8->f4=(void*)((_tmp6C4=_cycalloc(sizeof(*_tmp6C4)),((_tmp6C4[
0]=((_tmp6C5.tag=3,((_tmp6C5.f1=& Cyc_do_setjmp,_tmp6C5)))),_tmp6C4)))),((_tmp6C8->f5=((
_tmp6C3="Produce the jmp_buf and setjmp declarations on the standard output, for use by the Cyclone compiler special file cyc_setjmp.h.  Cannot be used with -gather, -gatherscript, or specfiles.",
_tag_dyneither(_tmp6C3,sizeof(char),186))),_tmp6C8)))))))))))),((_tmp6BC[3]=((
_tmp6CE=_cycalloc(sizeof(*_tmp6CE)),((_tmp6CE->f1=((_tmp6CD="-finish",
_tag_dyneither(_tmp6CD,sizeof(char),8))),((_tmp6CE->f2=0,((_tmp6CE->f3=((_tmp6CC="",
_tag_dyneither(_tmp6CC,sizeof(char),1))),((_tmp6CE->f4=(void*)((_tmp6CA=
_cycalloc(sizeof(*_tmp6CA)),((_tmp6CA[0]=((_tmp6CB.tag=0,((_tmp6CB.f1=Cyc_set_FINISH,
_tmp6CB)))),_tmp6CA)))),((_tmp6CE->f5=((_tmp6C9="Produce Cyclone headers from pre-gathered C library info",
_tag_dyneither(_tmp6C9,sizeof(char),57))),_tmp6CE)))))))))))),((_tmp6BC[2]=((
_tmp6D4=_cycalloc(sizeof(*_tmp6D4)),((_tmp6D4->f1=((_tmp6D3="-gatherscript",
_tag_dyneither(_tmp6D3,sizeof(char),14))),((_tmp6D4->f2=0,((_tmp6D4->f3=((
_tmp6D2="",_tag_dyneither(_tmp6D2,sizeof(char),1))),((_tmp6D4->f4=(void*)((
_tmp6D0=_cycalloc(sizeof(*_tmp6D0)),((_tmp6D0[0]=((_tmp6D1.tag=0,((_tmp6D1.f1=
Cyc_set_GATHERSCRIPT,_tmp6D1)))),_tmp6D0)))),((_tmp6D4->f5=((_tmp6CF="Produce a script to gather C library info",
_tag_dyneither(_tmp6CF,sizeof(char),42))),_tmp6D4)))))))))))),((_tmp6BC[1]=((
_tmp6DA=_cycalloc(sizeof(*_tmp6DA)),((_tmp6DA->f1=((_tmp6D9="-gather",
_tag_dyneither(_tmp6D9,sizeof(char),8))),((_tmp6DA->f2=0,((_tmp6DA->f3=((_tmp6D8="",
_tag_dyneither(_tmp6D8,sizeof(char),1))),((_tmp6DA->f4=(void*)((_tmp6D6=
_cycalloc(sizeof(*_tmp6D6)),((_tmp6D6[0]=((_tmp6D7.tag=0,((_tmp6D7.f1=Cyc_set_GATHER,
_tmp6D7)))),_tmp6D6)))),((_tmp6DA->f5=((_tmp6D5="Gather C library info but don't produce Cyclone headers",
_tag_dyneither(_tmp6D5,sizeof(char),56))),_tmp6DA)))))))))))),((_tmp6BC[0]=((
_tmp6E0=_cycalloc(sizeof(*_tmp6E0)),((_tmp6E0->f1=((_tmp6DF="-d",_tag_dyneither(
_tmp6DF,sizeof(char),3))),((_tmp6E0->f2=0,((_tmp6E0->f3=((_tmp6DE=" <file>",
_tag_dyneither(_tmp6DE,sizeof(char),8))),((_tmp6E0->f4=(void*)((_tmp6DC=
_cycalloc(sizeof(*_tmp6DC)),((_tmp6DC[0]=((_tmp6DD.tag=5,((_tmp6DD.f1=Cyc_set_output_dir,
_tmp6DD)))),_tmp6DC)))),((_tmp6E0->f5=((_tmp6DB="Set the output directory to <file>",
_tag_dyneither(_tmp6DB,sizeof(char),35))),_tmp6E0)))))))))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp6BC,sizeof(struct
_tuple24*),6)))))))))))));{const char*_tmp6E1;Cyc_Arg_parse(options,Cyc_add_spec_file,((
_tmp6E1="Options:",_tag_dyneither(_tmp6E1,sizeof(char),9))),argv);}if((((Cyc_badparse
 || !Cyc_do_setjmp  && Cyc_spec_files == 0) || Cyc_do_setjmp  && Cyc_spec_files != 0)
 || Cyc_do_setjmp  && Cyc_mode == Cyc_GATHER) || Cyc_do_setjmp  && Cyc_mode == Cyc_GATHERSCRIPT){{
const char*_tmp6E2;Cyc_Arg_usage(options,((_tmp6E2="Usage: buildlib [options] specfile1 specfile2 ...\nOptions:",
_tag_dyneither(_tmp6E2,sizeof(char),59))));}return 1;}if(Cyc_mode == Cyc_GATHERSCRIPT){
Cyc_script_file=Cyc_fopen((const char*)"BUILDLIB.sh",(const char*)"w");if(!((
unsigned int)Cyc_script_file)){{const char*_tmp6E5;void*_tmp6E4;(_tmp6E4=0,Cyc_fprintf(
Cyc_stderr,((_tmp6E5="Could not create file BUILDLIB.sh\n",_tag_dyneither(
_tmp6E5,sizeof(char),35))),_tag_dyneither(_tmp6E4,sizeof(void*),0)));}exit(1);}{
const char*_tmp6E8;void*_tmp6E7;(_tmp6E7=0,Cyc_prscript(((_tmp6E8="#!/bin/sh\n",
_tag_dyneither(_tmp6E8,sizeof(char),11))),_tag_dyneither(_tmp6E7,sizeof(void*),0)));}{
const char*_tmp6EB;void*_tmp6EA;(_tmp6EA=0,Cyc_prscript(((_tmp6EB="GCC=\"gcc\"\n",
_tag_dyneither(_tmp6EB,sizeof(char),11))),_tag_dyneither(_tmp6EA,sizeof(void*),0)));};}
if(Cyc_force_directory_prefixes(Cyc_output_dir) || Cyc_force_directory(Cyc_output_dir)){{
const char*_tmp6EF;void*_tmp6EE[1];struct Cyc_String_pa_struct _tmp6ED;(_tmp6ED.tag=
0,((_tmp6ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((
_tmp6EE[0]=& _tmp6ED,Cyc_fprintf(Cyc_stderr,((_tmp6EF="Error: could not create directory %s\n",
_tag_dyneither(_tmp6EF,sizeof(char),38))),_tag_dyneither(_tmp6EE,sizeof(void*),1)))))));}
return 1;}if(Cyc_mode == Cyc_GATHERSCRIPT){{const char*_tmp6F3;void*_tmp6F2[1];
struct Cyc_String_pa_struct _tmp6F1;(_tmp6F1.tag=0,((_tmp6F1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((_tmp6F2[0]=& _tmp6F1,Cyc_prscript(((
_tmp6F3="cd %s\n",_tag_dyneither(_tmp6F3,sizeof(char),7))),_tag_dyneither(
_tmp6F2,sizeof(void*),1)))))));}{const char*_tmp6F6;void*_tmp6F5;(_tmp6F5=0,Cyc_prscript(((
_tmp6F6="echo | $GCC -E -dM - -o INITMACROS.h\n",_tag_dyneither(_tmp6F6,sizeof(
char),38))),_tag_dyneither(_tmp6F5,sizeof(void*),0)));};}if(!Cyc_gathering()){{
const char*_tmp6F7;Cyc_log_file=Cyc_fopen((const char*)_untag_dyneither_ptr(Cyc_Filename_concat(
Cyc_output_dir,((_tmp6F7="BUILDLIB.LOG",_tag_dyneither(_tmp6F7,sizeof(char),13)))),
sizeof(char),1),(const char*)"w");}if(!((unsigned int)Cyc_log_file)){{const char*
_tmp6FB;void*_tmp6FA[1];struct Cyc_String_pa_struct _tmp6F9;(_tmp6F9.tag=0,((
_tmp6F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((
_tmp6FA[0]=& _tmp6F9,Cyc_fprintf(Cyc_stderr,((_tmp6FB="Error: could not create log file in directory %s\n",
_tag_dyneither(_tmp6FB,sizeof(char),50))),_tag_dyneither(_tmp6FA,sizeof(void*),1)))))));}
return 1;}if(!Cyc_do_setjmp){{const char*_tmp6FC;Cyc_cstubs_file=Cyc_fopen((const
char*)_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,((_tmp6FC="cstubs.c",
_tag_dyneither(_tmp6FC,sizeof(char),9)))),sizeof(char),1),(const char*)"w");}if(!((
unsigned int)Cyc_cstubs_file)){{const char*_tmp700;void*_tmp6FF[1];struct Cyc_String_pa_struct
_tmp6FE;(_tmp6FE.tag=0,((_tmp6FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_output_dir),((_tmp6FF[0]=& _tmp6FE,Cyc_fprintf(Cyc_stderr,((_tmp700="Error: could not create cstubs.c in directory %s\n",
_tag_dyneither(_tmp700,sizeof(char),50))),_tag_dyneither(_tmp6FF,sizeof(void*),1)))))));}
return 1;}{const char*_tmp701;Cyc_cycstubs_file=Cyc_fopen((const char*)
_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,((_tmp701="cycstubs.cyc",
_tag_dyneither(_tmp701,sizeof(char),13)))),sizeof(char),1),(const char*)"w");}if(
!((unsigned int)Cyc_cycstubs_file)){{const char*_tmp705;void*_tmp704[1];struct Cyc_String_pa_struct
_tmp703;(_tmp703.tag=0,((_tmp703.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_output_dir),((_tmp704[0]=& _tmp703,Cyc_fprintf(Cyc_stderr,((_tmp705="Error: could not create cycstubs.c in directory %s\n",
_tag_dyneither(_tmp705,sizeof(char),52))),_tag_dyneither(_tmp704,sizeof(void*),1)))))));}
return 1;}{const char*_tmp708;void*_tmp707;(_tmp707=0,Cyc_fprintf((struct Cyc___cycFILE*)
_check_null(Cyc_cycstubs_file),((_tmp708="#include <core.h>\nusing Core;\n\n",
_tag_dyneither(_tmp708,sizeof(char),32))),_tag_dyneither(_tmp707,sizeof(void*),0)));};}}{
const char*outdir=(const char*)_untag_dyneither_ptr(Cyc_output_dir,sizeof(char),1);
if(Cyc_do_setjmp  && Cyc_process_setjmp(outdir))return 1;else{for(0;Cyc_spec_files
!= 0;Cyc_spec_files=((struct Cyc_List_List*)_check_null(Cyc_spec_files))->tl){if(
Cyc_process_specfile((const char*)((struct Cyc_List_List*)_check_null(Cyc_spec_files))->hd,
outdir)){{const char*_tmp70B;void*_tmp70A;(_tmp70A=0,Cyc_fprintf(Cyc_stderr,((
_tmp70B="FATAL ERROR -- QUIT!\n",_tag_dyneither(_tmp70B,sizeof(char),22))),
_tag_dyneither(_tmp70A,sizeof(void*),0)));}exit(1);}}}if(Cyc_mode == Cyc_GATHERSCRIPT)
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_script_file));else{if(!Cyc_gathering()){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_log_file));if(!Cyc_do_setjmp){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file));Cyc_fclose((
struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file));}}}return 0;};};}
