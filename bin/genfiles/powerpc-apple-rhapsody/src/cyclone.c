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
char*tag;};struct _dyneither_ptr Cstring_to_string(char*);struct Cyc_List_List{void*
hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct
_dyneither_ptr);struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct
_dyneither_ptr);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*
tag;};struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{
char*tag;};void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,
void*x);struct _dyneither_ptr Cyc_List_to_array(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);extern char Cyc_Arg_Bad[4];
struct Cyc_Arg_Bad_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[
6];struct Cyc_Arg_Error_struct{char*tag;};struct Cyc_Arg_Unit_spec_struct{int tag;
void(*f1)();};struct Cyc_Arg_Flag_spec_struct{int tag;void(*f1)(struct
_dyneither_ptr);};struct Cyc_Arg_FlagString_spec_struct{int tag;void(*f1)(struct
_dyneither_ptr,struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_struct{int tag;int*
f1;};struct Cyc_Arg_Clear_spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_struct{
int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_Int_spec_struct{int tag;
void(*f1)(int);};struct Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct
_dyneither_ptr);};extern int Cyc_Arg_current;void Cyc_Arg_parse(struct Cyc_List_List*
specs,void(*anonfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct
_dyneither_ptr args);struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stdout;
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;}
;struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fclose(struct Cyc___cycFILE*);int Cyc_feof(struct Cyc___cycFILE*);
int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fgetc(struct Cyc___cycFILE*);struct Cyc___cycFILE*
Cyc_fopen(const char*,const char*);int Cyc_fprintf(struct Cyc___cycFILE*,struct
_dyneither_ptr,struct _dyneither_ptr);unsigned long Cyc_fread(struct _dyneither_ptr,
unsigned long,unsigned long,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
unsigned long Cyc_fwrite(struct _dyneither_ptr,unsigned long,unsigned long,struct
Cyc___cycFILE*);int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);int
remove(const char*);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};struct Cyc___cycFILE*Cyc_file_open(struct
_dyneither_ptr,struct _dyneither_ptr);void Cyc_file_close(struct Cyc___cycFILE*);
typedef struct{int quot;int rem;}Cyc_div_t;typedef struct{long quot;long rem;}Cyc_ldiv_t;
void*abort();void exit(int);char*getenv(const char*);int system(const char*);
unsigned long Cyc_strlen(struct _dyneither_ptr s);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);int Cyc_strptrcmp(struct _dyneither_ptr*
s1,struct _dyneither_ptr*s2);int Cyc_strncmp(struct _dyneither_ptr s1,struct
_dyneither_ptr s2,unsigned long len);struct _dyneither_ptr Cyc_strconcat(struct
_dyneither_ptr,struct _dyneither_ptr);struct _dyneither_ptr Cyc_strconcat_l(struct
Cyc_List_List*);struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct
_dyneither_ptr);struct _dyneither_ptr Cyc_strdup(struct _dyneither_ptr src);struct
_dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);struct
_dyneither_ptr Cyc_strchr(struct _dyneither_ptr s,char c);struct Cyc_Iter_Iter{void*
env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*
r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{
char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};
struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict d);struct _dyneither_ptr Cyc_Filename_concat(
struct _dyneither_ptr,struct _dyneither_ptr);struct _dyneither_ptr Cyc_Filename_chop_extension(
struct _dyneither_ptr);struct _dyneither_ptr Cyc_Filename_dirname(struct
_dyneither_ptr);struct _dyneither_ptr Cyc_Filename_basename(struct _dyneither_ptr);
int Cyc_Filename_check_suffix(struct _dyneither_ptr,struct _dyneither_ptr);struct
Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int
line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};void Cyc_Position_reset_position(struct _dyneither_ptr);struct Cyc_Position_Segment;
struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{int tag;};
struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{
char*tag;};extern int Cyc_Position_use_gcc_style_location;extern int Cyc_Position_max_errors;
int Cyc_Position_error_p();int isspace(int);struct _union_Nmspace_Rel_n{int tag;
struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace
Cyc_Absyn_Rel_n(struct Cyc_List_List*);union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(
struct Cyc_List_List*);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct
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
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct
Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;
struct _tuple1*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{
union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct
_tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};
struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct
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
f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple1*
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
_tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct
_tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct
_union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign
f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum 
Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7
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
int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple1*
f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple1*f1;};struct
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
int tag;struct Cyc_List_List*f1;};struct _tuple8{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple8*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
struct Cyc_Absyn_AnonEnum_e_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*
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
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;}
;struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;
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
int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;
};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;
struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;
void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{
struct _tuple1*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum 
Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct
_tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;
};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct
Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual
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
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};extern int Cyc_Absyn_porting_c_code;extern int Cyc_Absyn_no_regions;
extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_struct{char*tag;struct
_dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);
void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int
lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;
int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(
struct _dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct
_dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr
lex_check;};struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);
extern int Cyc_Parse_no_register;enum Cyc_Storage_class{Cyc_Typedef_sc  = 0,Cyc_Extern_sc
 = 1,Cyc_ExternC_sc  = 2,Cyc_Static_sc  = 3,Cyc_Auto_sc  = 4,Cyc_Register_sc  = 5,
Cyc_Abstract_sc  = 6};struct Cyc_Declaration_spec;struct Cyc_Declarator;struct Cyc_Abstractdeclarator;
struct _union_YYSTYPE_Int_tok{int tag;struct _tuple6 val;};struct
_union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;
struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*
val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple1*val;};struct _tuple10{
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
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};
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
timestamp;int first_line;int first_column;int last_line;int last_column;};struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);void Cyc_Absyndump_set_params(
struct Cyc_Absynpp_Params*fs);void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*
tdl,struct Cyc___cycFILE*f);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct
Cyc_Set_Absent_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tc_init(struct _RegionHandle*);void Cyc_Tcutil_flush_warnings();void Cyc_Tc_tc(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*
ds);struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);extern int Cyc_Tovc_elim_array_initializers;
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*decls);struct Cyc_CfFlowInfo_VarRoot_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{int tag;
int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;
};struct Cyc_CfFlowInfo_UniquePlace{struct Cyc_CfFlowInfo_Place place;struct Cyc_List_List*
path;};enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL
 = 1,Cyc_CfFlowInfo_AllIL  = 2};struct _union_RelnOp_EqualConst{int tag;
unsigned int val;};struct _tuple16{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct
_union_RelnOp_LessVar{int tag;struct _tuple16 val;};struct _union_RelnOp_LessNumelts{
int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessConst{int tag;
unsigned int val;};struct _union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*
val;};union Cyc_CfFlowInfo_RelnOp{struct _union_RelnOp_EqualConst EqualConst;struct
_union_RelnOp_LessVar LessVar;struct _union_RelnOp_LessNumelts LessNumelts;struct
_union_RelnOp_LessConst LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;
};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp
rop;};struct Cyc_CfFlowInfo_TagCmp{enum Cyc_Absyn_Primop cmp;void*bd;};extern char
Cyc_CfFlowInfo_HasTagCmps[11];struct Cyc_CfFlowInfo_HasTagCmps_struct{char*tag;
struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_struct{
char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_struct{
char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*
val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{
struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct
Cyc_CfFlowInfo_Zero_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_struct{int
tag;};struct Cyc_CfFlowInfo_NotZeroThis_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_struct{
int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;
enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;
struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;int f1;
struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_ConsumeInfo{struct Cyc_Dict_Dict
consumed;struct Cyc_List_List*may_consume;};struct _union_FlowInfo_BottomFL{int tag;
int val;};struct _tuple17{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo
f3;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple17 val;};union Cyc_CfFlowInfo_FlowInfo{
struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL
ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*
notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;
void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct
Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};struct Cyc_CfFlowInfo_UniqueRgn_k_struct{
int tag;};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);struct Cyc_Interface_I;
struct Cyc_Interface_I*Cyc_Interface_empty();struct Cyc_Interface_I*Cyc_Interface_final();
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict ae);struct
_tuple18{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple18*info);struct Cyc_Interface_I*
Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*
la,struct Cyc_List_List*linfo);struct Cyc_Interface_I*Cyc_Interface_parse(struct
Cyc___cycFILE*);void Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*);extern int Cyc_Cyclone_tovc_r;
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};extern
enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;void Cyc_Port_port(struct Cyc_List_List*);
void Cyc_Lex_lex_init(int use_cyclone_keywords);int Cyc_Toc_warn_bounds_checks;int
Cyc_Toc_warn_all_null_deref;static int Cyc_pp_r=0;int Cyc_noexpand_r=0;static int Cyc_noshake_r=
0;static int Cyc_stop_after_cpp_r=0;static int Cyc_parseonly_r=0;static int Cyc_tc_r=0;
static int Cyc_cf_r=0;static int Cyc_noprint_r=0;static int Cyc_ic_r=0;static int Cyc_toc_r=
0;static int Cyc_stop_after_objectfile_r=0;static int Cyc_stop_after_asmfile_r=0;
static int Cyc_inline_functions_r=0;static int Cyc_elim_se_r=0;static int Cyc_v_r=0;
static int Cyc_save_temps_r=0;static int Cyc_save_c_r=0;static int Cyc_nogc_r=0;static
int Cyc_pa_r=0;static int Cyc_pg_r=0;static int Cyc_nocheck_r=0;static int Cyc_add_cyc_namespace_r=
1;static int Cyc_generate_line_directives_r=0;static int Cyc_print_full_evars_r=0;
static int Cyc_print_all_tvars_r=0;static int Cyc_print_all_kinds_r=0;static int Cyc_print_all_effects_r=
0;static int Cyc_nocyc_setjmp_r=0;static struct Cyc_List_List*Cyc_ccargs=0;static
void Cyc_add_ccarg(struct _dyneither_ptr s);static void Cyc_add_ccarg(struct
_dyneither_ptr s){struct _dyneither_ptr*_tmp368;struct Cyc_List_List*_tmp367;Cyc_ccargs=((
_tmp367=_cycalloc(sizeof(*_tmp367)),((_tmp367->hd=((_tmp368=_cycalloc(sizeof(*
_tmp368)),((_tmp368[0]=s,_tmp368)))),((_tmp367->tl=Cyc_ccargs,_tmp367))))));}
void Cyc_set_c_compiler(struct _dyneither_ptr s);void Cyc_set_c_compiler(struct
_dyneither_ptr s){const char*_tmp369;if(Cyc_strcmp((struct _dyneither_ptr)s,((
_tmp369="vc",_tag_dyneither(_tmp369,sizeof(char),3))))== 0){Cyc_Cyclone_c_compiler=
Cyc_Cyclone_Vc_c;{const char*_tmp36A;Cyc_add_ccarg(((_tmp36A="-DVC_C",
_tag_dyneither(_tmp36A,sizeof(char),7))));};}else{const char*_tmp36B;if(Cyc_strcmp((
struct _dyneither_ptr)s,((_tmp36B="gcc",_tag_dyneither(_tmp36B,sizeof(char),4))))
== 0){Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;{const char*_tmp36C;Cyc_add_ccarg(((
_tmp36C="-DGCC_C",_tag_dyneither(_tmp36C,sizeof(char),8))));};}}}void Cyc_set_port_c_code();
void Cyc_set_port_c_code(){Cyc_Absyn_porting_c_code=1;Cyc_Position_max_errors=
65535;Cyc_save_c_r=1;Cyc_parseonly_r=1;}static struct _dyneither_ptr*Cyc_output_file=
0;static void Cyc_set_output_file(struct _dyneither_ptr s);static void Cyc_set_output_file(
struct _dyneither_ptr s){struct _dyneither_ptr*_tmp36D;Cyc_output_file=((_tmp36D=
_cycalloc(sizeof(*_tmp36D)),((_tmp36D[0]=s,_tmp36D))));}extern char*Cdef_inc_path;
extern char*Cdef_lib_path;extern char*Carch;extern char*Ccomp;extern char*Cversion;
static char _tmp7[1]="";static struct _dyneither_ptr Cyc_cpp={_tmp7,_tmp7,_tmp7 + 1};
static void Cyc_set_cpp(struct _dyneither_ptr s);static void Cyc_set_cpp(struct
_dyneither_ptr s){Cyc_cpp=s;}static struct Cyc_List_List*Cyc_cppargs=0;static void
Cyc_add_cpparg(struct _dyneither_ptr s);static void Cyc_add_cpparg(struct
_dyneither_ptr s){struct _dyneither_ptr*_tmp370;struct Cyc_List_List*_tmp36F;Cyc_cppargs=((
_tmp36F=_cycalloc(sizeof(*_tmp36F)),((_tmp36F->hd=((_tmp370=_cycalloc(sizeof(*
_tmp370)),((_tmp370[0]=s,_tmp370)))),((_tmp36F->tl=Cyc_cppargs,_tmp36F))))));}
static struct _dyneither_ptr Cyc_target_arch=(struct _dyneither_ptr){(void*)0,(void*)
0,(void*)(0 + 0)};static void Cyc_set_target_arch(struct _dyneither_ptr s);static void
Cyc_set_target_arch(struct _dyneither_ptr s){Cyc_toc_r=1;Cyc_target_arch=s;}static
void Cyc_print_version();static void Cyc_print_version(){{const char*_tmp374;void*
_tmp373[1];struct Cyc_String_pa_struct _tmp372;(_tmp372.tag=0,((_tmp372.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cversion)),((_tmp373[0]=&
_tmp372,Cyc_printf(((_tmp374="The Cyclone compiler, version %s\n",_tag_dyneither(
_tmp374,sizeof(char),34))),_tag_dyneither(_tmp373,sizeof(void*),1)))))));}{const
char*_tmp379;void*_tmp378[2];struct Cyc_String_pa_struct _tmp377;struct Cyc_String_pa_struct
_tmp376;(_tmp376.tag=0,((_tmp376.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cstring_to_string(Ccomp)),((_tmp377.tag=0,((_tmp377.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cstring_to_string(Carch)),((_tmp378[0]=& _tmp377,((_tmp378[1]=&
_tmp376,Cyc_printf(((_tmp379="Compiled for architecture: %s CC=\"%s\"\n",
_tag_dyneither(_tmp379,sizeof(char),39))),_tag_dyneither(_tmp378,sizeof(void*),2)))))))))))));}{
const char*_tmp37D;void*_tmp37C[1];struct Cyc_String_pa_struct _tmp37B;(_tmp37B.tag=
0,((_tmp37B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(
Cdef_lib_path)),((_tmp37C[0]=& _tmp37B,Cyc_printf(((_tmp37D="Standard library directory: %s\n",
_tag_dyneither(_tmp37D,sizeof(char),32))),_tag_dyneither(_tmp37C,sizeof(void*),1)))))));}{
const char*_tmp381;void*_tmp380[1];struct Cyc_String_pa_struct _tmp37F;(_tmp37F.tag=
0,((_tmp37F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(
Cdef_inc_path)),((_tmp380[0]=& _tmp37F,Cyc_printf(((_tmp381="Standard include directory: %s\n",
_tag_dyneither(_tmp381,sizeof(char),32))),_tag_dyneither(_tmp380,sizeof(void*),1)))))));}
exit(0);}static int Cyc_is_cyclone_sourcefile(struct _dyneither_ptr s);static int Cyc_is_cyclone_sourcefile(
struct _dyneither_ptr s){unsigned long _tmp17=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp17 <= 4)return 0;else{const char*_tmp382;return Cyc_strcmp((struct
_dyneither_ptr)_dyneither_ptr_plus(s,sizeof(char),(int)(_tmp17 - 4)),((_tmp382=".cyc",
_tag_dyneither(_tmp382,sizeof(char),5))))== 0;}}static struct Cyc_List_List*Cyc_cyclone_exec_path=
0;static void Cyc_add_cyclone_exec_path(struct _dyneither_ptr s);static void Cyc_add_cyclone_exec_path(
struct _dyneither_ptr s){unsigned long _tmp19=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp19 <= 2)return;{struct _dyneither_ptr _tmp1A=(struct _dyneither_ptr)Cyc_substring((
struct _dyneither_ptr)s,2,_tmp19 - 2);struct _dyneither_ptr*_tmp385;struct Cyc_List_List*
_tmp384;Cyc_cyclone_exec_path=((_tmp384=_cycalloc(sizeof(*_tmp384)),((_tmp384->hd=((
_tmp385=_cycalloc(sizeof(*_tmp385)),((_tmp385[0]=_tmp1A,_tmp385)))),((_tmp384->tl=
Cyc_cyclone_exec_path,_tmp384))))));};}static struct Cyc_List_List*Cyc_libargs=0;
static void Cyc_add_libarg(struct _dyneither_ptr s);static void Cyc_add_libarg(struct
_dyneither_ptr s){const char*_tmp386;if(Cyc_strcmp((struct _dyneither_ptr)s,((
_tmp386="-lxml",_tag_dyneither(_tmp386,sizeof(char),6))))== 0)Cyc_add_ccarg(s);
else{struct _dyneither_ptr*_tmp389;struct Cyc_List_List*_tmp388;Cyc_libargs=((
_tmp388=_cycalloc(sizeof(*_tmp388)),((_tmp388->hd=((_tmp389=_cycalloc(sizeof(*
_tmp389)),((_tmp389[0]=s,_tmp389)))),((_tmp388->tl=Cyc_libargs,_tmp388))))));}}
static void Cyc_add_marg(struct _dyneither_ptr s);static void Cyc_add_marg(struct
_dyneither_ptr s){Cyc_add_ccarg(s);}static void Cyc_set_save_temps();static void Cyc_set_save_temps(){
Cyc_save_temps_r=1;{const char*_tmp38A;Cyc_add_ccarg(((_tmp38A="-save-temps",
_tag_dyneither(_tmp38A,sizeof(char),12))));};}static int Cyc_produce_dependencies=
0;static void Cyc_set_produce_dependencies();static void Cyc_set_produce_dependencies(){
Cyc_stop_after_cpp_r=1;Cyc_produce_dependencies=1;{const char*_tmp38B;Cyc_add_cpparg(((
_tmp38B="-M",_tag_dyneither(_tmp38B,sizeof(char),3))));};}static struct
_dyneither_ptr*Cyc_dependencies_target=0;static void Cyc_set_dependencies_target(
struct _dyneither_ptr s);static void Cyc_set_dependencies_target(struct
_dyneither_ptr s){struct _dyneither_ptr*_tmp38C;Cyc_dependencies_target=((_tmp38C=
_cycalloc(sizeof(*_tmp38C)),((_tmp38C[0]=s,_tmp38C))));}static void Cyc_set_stop_after_objectfile();
static void Cyc_set_stop_after_objectfile(){Cyc_stop_after_objectfile_r=1;{const
char*_tmp38D;Cyc_add_ccarg(((_tmp38D="-c",_tag_dyneither(_tmp38D,sizeof(char),3))));};}
static void Cyc_set_nocppprecomp();static void Cyc_set_nocppprecomp(){{const char*
_tmp38E;Cyc_add_cpparg(((_tmp38E="-no-cpp-precomp",_tag_dyneither(_tmp38E,
sizeof(char),16))));}{const char*_tmp38F;Cyc_add_ccarg(((_tmp38F="-no-cpp-precomp",
_tag_dyneither(_tmp38F,sizeof(char),16))));};}static void Cyc_set_lineno();static
void Cyc_set_lineno(){Cyc_generate_line_directives_r=1;Cyc_set_save_temps();}
static void Cyc_set_inline_functions();static void Cyc_set_inline_functions(){Cyc_inline_functions_r=
1;}static void Cyc_set_elim_se();static void Cyc_set_elim_se(){Cyc_elim_se_r=1;Cyc_set_inline_functions();}
static void Cyc_set_tovc();static void Cyc_set_tovc(){Cyc_Cyclone_tovc_r=1;{const
char*_tmp390;Cyc_add_ccarg(((_tmp390="-DCYC_ANSI_OUTPUT",_tag_dyneither(_tmp390,
sizeof(char),18))));}Cyc_set_elim_se();}static void Cyc_set_noboundschecks();
static void Cyc_set_noboundschecks(){const char*_tmp391;Cyc_add_ccarg(((_tmp391="-DNO_CYC_BOUNDS_CHECKS",
_tag_dyneither(_tmp391,sizeof(char),23))));}static void Cyc_set_nonullchecks();
static void Cyc_set_nonullchecks(){const char*_tmp392;Cyc_add_ccarg(((_tmp392="-DNO_CYC_NULL_CHECKS",
_tag_dyneither(_tmp392,sizeof(char),21))));}static void Cyc_set_nochecks();static
void Cyc_set_nochecks(){Cyc_set_noboundschecks();Cyc_set_nonullchecks();Cyc_nocheck_r=
1;}static void Cyc_set_nocyc();static void Cyc_set_nocyc(){Cyc_add_cyc_namespace_r=0;{
const char*_tmp393;Cyc_add_ccarg(((_tmp393="-DNO_CYC_PREFIX",_tag_dyneither(
_tmp393,sizeof(char),16))));};}static void Cyc_set_pa();static void Cyc_set_pa(){Cyc_pa_r=
1;{const char*_tmp394;Cyc_add_ccarg(((_tmp394="-DCYC_REGION_PROFILE",
_tag_dyneither(_tmp394,sizeof(char),21))));};}static void Cyc_set_pg();static void
Cyc_set_pg(){Cyc_pg_r=1;{const char*_tmp395;Cyc_add_ccarg(((_tmp395="-pg",
_tag_dyneither(_tmp395,sizeof(char),4))));};}static void Cyc_set_stop_after_asmfile();
static void Cyc_set_stop_after_asmfile(){Cyc_stop_after_asmfile_r=1;{const char*
_tmp396;Cyc_add_ccarg(((_tmp396="-S",_tag_dyneither(_tmp396,sizeof(char),3))));};}
static void Cyc_set_all_warnings();static void Cyc_set_all_warnings(){Cyc_Toc_warn_bounds_checks=
1;Cyc_Toc_warn_all_null_deref=1;}enum Cyc_inputtype{Cyc_DEFAULTINPUT  = 0,Cyc_CYCLONEFILE
 = 1};static enum Cyc_inputtype Cyc_intype=Cyc_DEFAULTINPUT;static void Cyc_set_inputtype(
struct _dyneither_ptr s);static void Cyc_set_inputtype(struct _dyneither_ptr s){const
char*_tmp397;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp397="cyc",
_tag_dyneither(_tmp397,sizeof(char),4))))== 0)Cyc_intype=Cyc_CYCLONEFILE;else{
const char*_tmp398;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp398="none",
_tag_dyneither(_tmp398,sizeof(char),5))))== 0)Cyc_intype=Cyc_DEFAULTINPUT;else{
const char*_tmp39C;void*_tmp39B[1];struct Cyc_String_pa_struct _tmp39A;(_tmp39A.tag=
0,((_tmp39A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp39B[0]=&
_tmp39A,Cyc_fprintf(Cyc_stderr,((_tmp39C="Input type '%s' not supported\n",
_tag_dyneither(_tmp39C,sizeof(char),31))),_tag_dyneither(_tmp39B,sizeof(void*),1)))))));}}}
struct _dyneither_ptr Cyc_make_base_filename(struct _dyneither_ptr s,struct
_dyneither_ptr*output_file);struct _dyneither_ptr Cyc_make_base_filename(struct
_dyneither_ptr s,struct _dyneither_ptr*output_file){struct _dyneither_ptr _tmp32=Cyc_Filename_dirname(
output_file == 0?s:*output_file);struct _dyneither_ptr _tmp33=Cyc_Filename_chop_extension((
struct _dyneither_ptr)Cyc_Filename_basename(s));struct _dyneither_ptr _tmp34=Cyc_strlen((
struct _dyneither_ptr)_tmp32)> 0?Cyc_Filename_concat((struct _dyneither_ptr)_tmp32,(
struct _dyneither_ptr)_tmp33): _tmp33;return(struct _dyneither_ptr)_tmp34;}static
struct Cyc_List_List*Cyc_cyclone_files=0;static void Cyc_add_other(struct
_dyneither_ptr s);static void Cyc_add_other(struct _dyneither_ptr s){if(Cyc_intype == 
Cyc_CYCLONEFILE  || Cyc_is_cyclone_sourcefile(s)){{struct _dyneither_ptr*_tmp39F;
struct Cyc_List_List*_tmp39E;Cyc_cyclone_files=((_tmp39E=_cycalloc(sizeof(*
_tmp39E)),((_tmp39E->hd=((_tmp39F=_cycalloc(sizeof(*_tmp39F)),((_tmp39F[0]=s,
_tmp39F)))),((_tmp39E->tl=Cyc_cyclone_files,_tmp39E))))));}{struct _dyneither_ptr
_tmp37=Cyc_make_base_filename(s,Cyc_output_file);const char*_tmp3A0;struct
_dyneither_ptr _tmp38=Cyc_strconcat((struct _dyneither_ptr)_tmp37,((_tmp3A0=".c",
_tag_dyneither(_tmp3A0,sizeof(char),3))));Cyc_add_ccarg((struct _dyneither_ptr)
_tmp38);};}else{Cyc_add_ccarg(s);}}static void Cyc_remove_file(struct
_dyneither_ptr s);static void Cyc_remove_file(struct _dyneither_ptr s){if(Cyc_save_temps_r)
return;else{remove((const char*)_untag_dyneither_ptr(s,sizeof(char),1));}}struct
_tuple19{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;};struct Cyc_List_List*
Cyc_read_specs(struct _dyneither_ptr file);struct Cyc_List_List*Cyc_read_specs(
struct _dyneither_ptr file){struct Cyc_List_List*_tmp3A=0;int c;int i;char strname[256];
char strvalue[4096];struct Cyc___cycFILE*spec_file=Cyc_fopen((const char*)
_untag_dyneither_ptr(file,sizeof(char),1),(const char*)"r");if(spec_file == 0){{
const char*_tmp3A4;void*_tmp3A3[1];struct Cyc_String_pa_struct _tmp3A2;(_tmp3A2.tag=
0,((_tmp3A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp3A3[0]=&
_tmp3A2,Cyc_fprintf(Cyc_stderr,((_tmp3A4="Error opening spec file %s\n",
_tag_dyneither(_tmp3A4,sizeof(char),28))),_tag_dyneither(_tmp3A3,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 0;}while(1){while(1){c=Cyc_fgetc((
struct Cyc___cycFILE*)spec_file);if(isspace(c))continue;if(c == '*')break;if(c != -
1){{const char*_tmp3A9;void*_tmp3A8[2];struct Cyc_String_pa_struct _tmp3A7;struct
Cyc_Int_pa_struct _tmp3A6;(_tmp3A6.tag=1,((_tmp3A6.f1=(unsigned long)c,((_tmp3A7.tag=
0,((_tmp3A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp3A8[0]=&
_tmp3A7,((_tmp3A8[1]=& _tmp3A6,Cyc_fprintf(Cyc_stderr,((_tmp3A9="Error reading spec file %s: unexpected character '%c'\n",
_tag_dyneither(_tmp3A9,sizeof(char),55))),_tag_dyneither(_tmp3A8,sizeof(void*),2)))))))))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}goto CLEANUP_AND_RETURN;}
JUST_AFTER_STAR: i=0;while(1){c=Cyc_fgetc((struct Cyc___cycFILE*)spec_file);if(c == 
- 1){{const char*_tmp3AD;void*_tmp3AC[1];struct Cyc_String_pa_struct _tmp3AB;(
_tmp3AB.tag=0,((_tmp3AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((
_tmp3AC[0]=& _tmp3AB,Cyc_fprintf(Cyc_stderr,((_tmp3AD="Error reading spec file %s: unexpected EOF\n",
_tag_dyneither(_tmp3AD,sizeof(char),44))),_tag_dyneither(_tmp3AC,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);goto CLEANUP_AND_RETURN;}if(c == ':'){
strname[_check_known_subscript_notnull(256,i)]='\000';break;}strname[
_check_known_subscript_notnull(256,i)]=(char)c;++ i;if(i >= 256){{const char*
_tmp3B1;void*_tmp3B0[1];struct Cyc_String_pa_struct _tmp3AF;(_tmp3AF.tag=0,((
_tmp3AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp3B0[0]=&
_tmp3AF,Cyc_fprintf(Cyc_stderr,((_tmp3B1="Error reading spec file %s: string name too long\n",
_tag_dyneither(_tmp3B1,sizeof(char),50))),_tag_dyneither(_tmp3B0,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);goto CLEANUP_AND_RETURN;}}while(1){c=
Cyc_fgetc((struct Cyc___cycFILE*)spec_file);if(isspace(c))continue;break;}if(c == '*'){{
struct _tuple19*_tmp3BE;struct _dyneither_ptr*_tmp3BD;const char*_tmp3BC;struct
_dyneither_ptr*_tmp3BB;struct Cyc_List_List*_tmp3BA;_tmp3A=((_tmp3BA=_cycalloc(
sizeof(*_tmp3BA)),((_tmp3BA->hd=((_tmp3BE=_cycalloc(sizeof(*_tmp3BE)),((_tmp3BE->f1=((
_tmp3BD=_cycalloc(sizeof(*_tmp3BD)),((_tmp3BD[0]=(struct _dyneither_ptr)Cyc_strdup(
_tag_dyneither(strname,sizeof(char),256)),_tmp3BD)))),((_tmp3BE->f2=((_tmp3BB=
_cycalloc(sizeof(*_tmp3BB)),((_tmp3BB[0]=(struct _dyneither_ptr)Cyc_strdup(((
_tmp3BC="",_tag_dyneither(_tmp3BC,sizeof(char),1)))),_tmp3BB)))),_tmp3BE)))))),((
_tmp3BA->tl=_tmp3A,_tmp3BA))))));}goto JUST_AFTER_STAR;}strvalue[0]=(char)c;i=1;
while(1){c=Cyc_fgetc((struct Cyc___cycFILE*)spec_file);if((c == - 1  || c == '\n')
 || c == '\r'){strvalue[_check_known_subscript_notnull(4096,i)]='\000';break;}
strvalue[_check_known_subscript_notnull(4096,i)]=(char)c;++ i;if(i >= 4096){{const
char*_tmp3C3;void*_tmp3C2[2];struct Cyc_String_pa_struct _tmp3C1;struct Cyc_String_pa_struct
_tmp3C0;(_tmp3C0.tag=0,((_tmp3C0.f1=(struct _dyneither_ptr)_tag_dyneither(strname,
sizeof(char),256),((_tmp3C1.tag=0,((_tmp3C1.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)file),((_tmp3C2[0]=& _tmp3C1,((_tmp3C2[1]=& _tmp3C0,Cyc_fprintf(Cyc_stderr,((
_tmp3C3="Error reading spec file %s: value of %s too long\n",_tag_dyneither(
_tmp3C3,sizeof(char),50))),_tag_dyneither(_tmp3C2,sizeof(void*),2)))))))))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);goto CLEANUP_AND_RETURN;}}{struct
_tuple19*_tmp3CC;struct _dyneither_ptr*_tmp3CB;struct _dyneither_ptr*_tmp3CA;
struct Cyc_List_List*_tmp3C9;_tmp3A=((_tmp3C9=_cycalloc(sizeof(*_tmp3C9)),((
_tmp3C9->hd=((_tmp3CC=_cycalloc(sizeof(*_tmp3CC)),((_tmp3CC->f1=((_tmp3CB=
_cycalloc(sizeof(*_tmp3CB)),((_tmp3CB[0]=(struct _dyneither_ptr)Cyc_strdup(
_tag_dyneither(strname,sizeof(char),256)),_tmp3CB)))),((_tmp3CC->f2=((_tmp3CA=
_cycalloc(sizeof(*_tmp3CA)),((_tmp3CA[0]=(struct _dyneither_ptr)Cyc_strdup(
_tag_dyneither(strvalue,sizeof(char),4096)),_tmp3CA)))),_tmp3CC)))))),((_tmp3C9->tl=
_tmp3A,_tmp3C9))))));}if(c == - 1)goto CLEANUP_AND_RETURN;}CLEANUP_AND_RETURN: Cyc_fclose((
struct Cyc___cycFILE*)spec_file);return _tmp3A;}struct _dyneither_ptr Cyc_split_specs(
struct _dyneither_ptr cmdline);static void _tmp3D6(struct _dyneither_ptr*_tmp5B,
unsigned int*_tmp3D5,unsigned int*_tmp3D4,struct _dyneither_ptr**_tmp3D2){for(*
_tmp3D5=0;*_tmp3D5 < *_tmp3D4;(*_tmp3D5)++){(*_tmp3D2)[*_tmp3D5]=*((struct
_dyneither_ptr**)(*_tmp5B).curr)[(int)*_tmp3D5];}}struct _dyneither_ptr Cyc_split_specs(
struct _dyneither_ptr cmdline){if(cmdline.curr == (_tag_dyneither(0,0,0)).curr)
return _tag_dyneither(0,0,0);{unsigned long _tmp55=Cyc_strlen((struct
_dyneither_ptr)cmdline);struct Cyc_List_List*_tmp56=0;char buf[4096];int i=0;int j=0;
if(_tmp55 > 4096)goto DONE;while(1){while(1){if(i >= _tmp55)goto DONE;if(*((const
char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)goto DONE;if(!
isspace((int)*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))
break;++ i;}j=0;while(1){if(i >= _tmp55)break;if(*((const char*)
_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)break;if(isspace((int)*((
const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))break;if(*((
const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== '\\'){++ i;if(i
>= _tmp55)break;if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),
i))== 0)break;buf[_check_known_subscript_notnull(4096,j)]=*((const char*)
_check_dyneither_subscript(cmdline,sizeof(char),i));++ j;}else{buf[
_check_known_subscript_notnull(4096,j)]=*((const char*)_check_dyneither_subscript(
cmdline,sizeof(char),i));++ j;}++ i;}if(j < 4096)buf[_check_known_subscript_notnull(
4096,j)]='\000';{struct _dyneither_ptr*_tmp3CF;struct Cyc_List_List*_tmp3CE;_tmp56=((
_tmp3CE=_cycalloc(sizeof(*_tmp3CE)),((_tmp3CE->hd=((_tmp3CF=_cycalloc(sizeof(*
_tmp3CF)),((_tmp3CF[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(buf,
sizeof(char),4096)),_tmp3CF)))),((_tmp3CE->tl=_tmp56,_tmp3CE))))));};}DONE:
_tmp56=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp56);{
struct Cyc_List_List*_tmp3D0;_tmp56=((_tmp3D0=_cycalloc(sizeof(*_tmp3D0)),((
_tmp3D0->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",
sizeof(char),1),((_tmp3D0->tl=_tmp56,_tmp3D0))))));}{struct _dyneither_ptr _tmp5B=((
struct _dyneither_ptr(*)(struct Cyc_List_List*x))Cyc_List_to_array)(_tmp56);
unsigned int _tmp3D5;unsigned int _tmp3D4;struct _dyneither_ptr _tmp3D3;struct
_dyneither_ptr*_tmp3D2;unsigned int _tmp3D1;struct _dyneither_ptr _tmp5C=(_tmp3D1=
_get_dyneither_size(_tmp5B,sizeof(struct _dyneither_ptr*)),((_tmp3D2=(struct
_dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp3D1)),((
_tmp3D3=_tag_dyneither(_tmp3D2,sizeof(struct _dyneither_ptr),_tmp3D1),((((_tmp3D4=
_tmp3D1,_tmp3D6(& _tmp5B,& _tmp3D5,& _tmp3D4,& _tmp3D2))),_tmp3D3)))))));return
_tmp5C;};};}int Cyc_compile_failure=0;struct Cyc___cycFILE*Cyc_try_file_open(
struct _dyneither_ptr filename,struct _dyneither_ptr mode,struct _dyneither_ptr
msg_part);struct Cyc___cycFILE*Cyc_try_file_open(struct _dyneither_ptr filename,
struct _dyneither_ptr mode,struct _dyneither_ptr msg_part){struct _handler_cons _tmp61;
_push_handler(& _tmp61);{int _tmp63=0;if(setjmp(_tmp61.handler))_tmp63=1;if(!
_tmp63){{struct Cyc___cycFILE*_tmp64=(struct Cyc___cycFILE*)Cyc_file_open(filename,
mode);_npop_handler(0);return _tmp64;};_pop_handler();}else{void*_tmp62=(void*)
_exn_thrown;void*_tmp66=_tmp62;_LL1:;_LL2: Cyc_compile_failure=1;{const char*
_tmp3DB;void*_tmp3DA[2];struct Cyc_String_pa_struct _tmp3D9;struct Cyc_String_pa_struct
_tmp3D8;(_tmp3D8.tag=0,((_tmp3D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
filename),((_tmp3D9.tag=0,((_tmp3D9.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)msg_part),((_tmp3DA[0]=& _tmp3D9,((_tmp3DA[1]=& _tmp3D8,Cyc_fprintf(
Cyc_stderr,((_tmp3DB="\nError: couldn't open %s %s\n",_tag_dyneither(_tmp3DB,
sizeof(char),29))),_tag_dyneither(_tmp3DA,sizeof(void*),2)))))))))))));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);return 0;_LL3:;_LL4:(void)_throw(_tmp66);_LL0:;}};}
struct Cyc_List_List*Cyc_do_stage(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*
f)(void*,struct Cyc_List_List*),void*env,struct Cyc_List_List*tds);struct Cyc_List_List*
Cyc_do_stage(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(void*,
struct Cyc_List_List*),void*env,struct Cyc_List_List*tds){const char*_tmp3DC;struct
_dyneither_ptr exn_string=(_tmp3DC="",_tag_dyneither(_tmp3DC,sizeof(char),1));
const char*_tmp3DD;struct _dyneither_ptr explain_string=(_tmp3DD="",_tag_dyneither(
_tmp3DD,sizeof(char),1));int other_exn=0;struct Cyc_Core_Impossible_struct _tmp3E3;
const char*_tmp3E2;struct Cyc_Core_Impossible_struct*_tmp3E1;void*ex=(void*)((
_tmp3E1=_cycalloc(sizeof(*_tmp3E1)),((_tmp3E1[0]=((_tmp3E3.tag=Cyc_Core_Impossible,((
_tmp3E3.f1=((_tmp3E2="",_tag_dyneither(_tmp3E2,sizeof(char),1))),_tmp3E3)))),
_tmp3E1))));struct Cyc_List_List*_tmp6B=0;{struct _handler_cons _tmp6C;
_push_handler(& _tmp6C);{int _tmp6E=0;if(setjmp(_tmp6C.handler))_tmp6E=1;if(!
_tmp6E){_tmp6B=f(env,tds);;_pop_handler();}else{void*_tmp6D=(void*)_exn_thrown;
void*_tmp70=_tmp6D;struct _dyneither_ptr _tmp72;struct _dyneither_ptr _tmp75;struct
_dyneither_ptr _tmp77;_LL6: {struct Cyc_Core_Impossible_struct*_tmp71=(struct Cyc_Core_Impossible_struct*)
_tmp70;if(_tmp71->tag != Cyc_Core_Impossible)goto _LL8;else{_tmp72=_tmp71->f1;}}
_LL7:{const char*_tmp3E4;exn_string=((_tmp3E4="Exception Core::Impossible",
_tag_dyneither(_tmp3E4,sizeof(char),27)));}explain_string=_tmp72;goto _LL5;_LL8: {
struct Cyc_Dict_Absent_struct*_tmp73=(struct Cyc_Dict_Absent_struct*)_tmp70;if(
_tmp73->tag != Cyc_Dict_Absent)goto _LLA;}_LL9:{const char*_tmp3E5;exn_string=((
_tmp3E5="Exception Dict::Absent",_tag_dyneither(_tmp3E5,sizeof(char),23)));}goto
_LL5;_LLA: {struct Cyc_Core_Invalid_argument_struct*_tmp74=(struct Cyc_Core_Invalid_argument_struct*)
_tmp70;if(_tmp74->tag != Cyc_Core_Invalid_argument)goto _LLC;else{_tmp75=_tmp74->f1;}}
_LLB:{const char*_tmp3E6;exn_string=((_tmp3E6="Exception Core::Invalid_argument",
_tag_dyneither(_tmp3E6,sizeof(char),33)));}explain_string=_tmp75;goto _LL5;_LLC: {
struct Cyc_Core_Failure_struct*_tmp76=(struct Cyc_Core_Failure_struct*)_tmp70;if(
_tmp76->tag != Cyc_Core_Failure)goto _LLE;else{_tmp77=_tmp76->f1;}}_LLD:{const char*
_tmp3E7;exn_string=((_tmp3E7="Exception Core::Failure",_tag_dyneither(_tmp3E7,
sizeof(char),24)));}explain_string=_tmp77;goto _LL5;_LLE:;_LLF: ex=_tmp70;
other_exn=1;{const char*_tmp3E8;exn_string=((_tmp3E8="Uncaught exception",
_tag_dyneither(_tmp3E8,sizeof(char),19)));}goto _LL5;_LL10:;_LL11:(void)_throw(
_tmp70);_LL5:;}};}if(Cyc_Position_error_p())Cyc_compile_failure=1;{const char*
_tmp3E9;if(Cyc_strcmp((struct _dyneither_ptr)exn_string,((_tmp3E9="",
_tag_dyneither(_tmp3E9,sizeof(char),1))))!= 0){Cyc_compile_failure=1;{const char*
_tmp3EF;void*_tmp3EE[3];struct Cyc_String_pa_struct _tmp3ED;struct Cyc_String_pa_struct
_tmp3EC;struct Cyc_String_pa_struct _tmp3EB;(_tmp3EB.tag=0,((_tmp3EB.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)explain_string),((_tmp3EC.tag=0,((_tmp3EC.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)stage_name),((_tmp3ED.tag=0,((
_tmp3ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)exn_string),((_tmp3EE[0]=&
_tmp3ED,((_tmp3EE[1]=& _tmp3EC,((_tmp3EE[2]=& _tmp3EB,Cyc_fprintf(Cyc_stderr,((
_tmp3EF="\n%s thrown during %s: %s",_tag_dyneither(_tmp3EF,sizeof(char),25))),
_tag_dyneither(_tmp3EE,sizeof(void*),3)))))))))))))))))));};}}if(Cyc_compile_failure){{
const char*_tmp3F2;void*_tmp3F1;(_tmp3F1=0,Cyc_fprintf(Cyc_stderr,((_tmp3F2="\nCOMPILATION FAILED!\n",
_tag_dyneither(_tmp3F2,sizeof(char),22))),_tag_dyneither(_tmp3F1,sizeof(void*),0)));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);if(other_exn)(int)_throw(ex);return
_tmp6B;}else{if(Cyc_v_r){{const char*_tmp3F6;void*_tmp3F5[1];struct Cyc_String_pa_struct
_tmp3F4;(_tmp3F4.tag=0,((_tmp3F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
stage_name),((_tmp3F5[0]=& _tmp3F4,Cyc_fprintf(Cyc_stderr,((_tmp3F6="%s completed.\n",
_tag_dyneither(_tmp3F6,sizeof(char),15))),_tag_dyneither(_tmp3F5,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return _tmp6B;}}return _tmp6B;}struct
Cyc_List_List*Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*ignore);
struct Cyc_List_List*Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*
ignore){Cyc_Lex_lex_init(1);{struct Cyc_List_List*_tmp8D=Cyc_Parse_parse_file(f);
Cyc_Lex_lex_init(1);return _tmp8D;};}struct _tuple20{struct _RegionHandle*f1;struct
Cyc_Tcenv_Tenv*f2;};struct Cyc_List_List*Cyc_do_typecheck(struct _tuple20*tcenv,
struct Cyc_List_List*tds);struct Cyc_List_List*Cyc_do_typecheck(struct _tuple20*
tcenv,struct Cyc_List_List*tds){struct _RegionHandle*_tmp8F;struct Cyc_Tcenv_Tenv*
_tmp90;struct _tuple20 _tmp8E=*tcenv;_tmp8F=_tmp8E.f1;_tmp90=_tmp8E.f2;Cyc_Tc_tc(
_tmp8F,_tmp90,1,tds);if(!Cyc_noshake_r)tds=Cyc_Tc_treeshake(_tmp90,tds);return
tds;}struct Cyc_List_List*Cyc_do_cfcheck(int ignore,struct Cyc_List_List*tds);
struct Cyc_List_List*Cyc_do_cfcheck(int ignore,struct Cyc_List_List*tds){Cyc_NewControlFlow_cf_check(
tds);return tds;}struct _tuple21{struct Cyc_Tcenv_Tenv*f1;struct Cyc___cycFILE*f2;
struct Cyc___cycFILE*f3;};struct Cyc_List_List*Cyc_do_interface(struct _tuple21*
params,struct Cyc_List_List*tds);struct Cyc_List_List*Cyc_do_interface(struct
_tuple21*params,struct Cyc_List_List*tds){struct _tuple21 _tmp92;struct Cyc_Tcenv_Tenv*
_tmp93;struct Cyc___cycFILE*_tmp94;struct Cyc___cycFILE*_tmp95;struct _tuple21*
_tmp91=params;_tmp92=*_tmp91;_tmp93=_tmp92.f1;_tmp94=_tmp92.f2;_tmp95=_tmp92.f3;{
struct Cyc_Interface_I*_tmp96=Cyc_Interface_extract(_tmp93->ae);if(_tmp94 == 0)Cyc_Interface_save(
_tmp96,_tmp95);else{struct Cyc_Interface_I*_tmp97=Cyc_Interface_parse((struct Cyc___cycFILE*)
_tmp94);const char*_tmp3FB;const char*_tmp3FA;struct _tuple18*_tmp3F9;if(!Cyc_Interface_is_subinterface(
_tmp97,_tmp96,((_tmp3F9=_cycalloc(sizeof(*_tmp3F9)),((_tmp3F9->f1=((_tmp3FA="written interface",
_tag_dyneither(_tmp3FA,sizeof(char),18))),((_tmp3F9->f2=((_tmp3FB="maximal interface",
_tag_dyneither(_tmp3FB,sizeof(char),18))),_tmp3F9))))))))Cyc_compile_failure=1;
else{Cyc_Interface_save(_tmp97,_tmp95);}}return tds;};}struct Cyc_List_List*Cyc_do_translate(
int ignore,struct Cyc_List_List*tds);struct Cyc_List_List*Cyc_do_translate(int
ignore,struct Cyc_List_List*tds){return Cyc_Toc_toc(tds);}struct Cyc_List_List*Cyc_do_tovc(
int ignore,struct Cyc_List_List*tds);struct Cyc_List_List*Cyc_do_tovc(int ignore,
struct Cyc_List_List*tds){Cyc_Tovc_elim_array_initializers=Cyc_Cyclone_tovc_r;
return Cyc_Tovc_tovc(tds);}static struct _dyneither_ptr Cyc_cyc_setjmp=(struct
_dyneither_ptr){(void*)0,(void*)0,(void*)(0 + 0)};static struct _dyneither_ptr Cyc_cyc_include=(
struct _dyneither_ptr){(void*)0,(void*)0,(void*)(0 + 0)};static void Cyc_set_cyc_include(
struct _dyneither_ptr f);static void Cyc_set_cyc_include(struct _dyneither_ptr f){Cyc_cyc_include=
f;}int Cyc_copy_internal_file(struct _dyneither_ptr file,struct Cyc___cycFILE*
out_file);int Cyc_copy_internal_file(struct _dyneither_ptr file,struct Cyc___cycFILE*
out_file){if(file.curr == (_tag_dyneither(0,0,0)).curr){{const char*_tmp3FE;void*
_tmp3FD;(_tmp3FD=0,Cyc_fprintf(Cyc_stderr,((_tmp3FE="Internal error: copy_internal_file called with NULL\n",
_tag_dyneither(_tmp3FE,sizeof(char),53))),_tag_dyneither(_tmp3FD,sizeof(void*),0)));}
return 1;}{const char*_tmp400;const char*_tmp3FF;struct Cyc___cycFILE*_tmp9D=Cyc_try_file_open(
file,((_tmp3FF="r",_tag_dyneither(_tmp3FF,sizeof(char),2))),((_tmp400="internal compiler file",
_tag_dyneither(_tmp400,sizeof(char),23))));if(_tmp9D == 0)return 1;{unsigned long
n_read=1024;unsigned long n_written;char buf[1024];{unsigned int _tmpA4=1024;
unsigned int i;for(i=0;i < _tmpA4;i ++){buf[i]='\000';}}while(n_read == 1024){n_read=
Cyc_fread(_tag_dyneither(buf,sizeof(char),1024),1,1024,(struct Cyc___cycFILE*)
_tmp9D);if(n_read != 1024  && !Cyc_feof((struct Cyc___cycFILE*)_tmp9D)){Cyc_compile_failure=
1;{const char*_tmp404;void*_tmp403[1];struct Cyc_String_pa_struct _tmp402;(_tmp402.tag=
0,((_tmp402.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp403[0]=&
_tmp402,Cyc_fprintf(Cyc_stderr,((_tmp404="\nError: problem copying %s\n",
_tag_dyneither(_tmp404,sizeof(char),28))),_tag_dyneither(_tmp403,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 1;}n_written=Cyc_fwrite(
_tag_dyneither(buf,sizeof(char),1024),1,n_read,out_file);if(n_read != n_written){
Cyc_compile_failure=1;{const char*_tmp408;void*_tmp407[1];struct Cyc_String_pa_struct
_tmp406;(_tmp406.tag=0,((_tmp406.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
file),((_tmp407[0]=& _tmp406,Cyc_fprintf(Cyc_stderr,((_tmp408="\nError: problem copying %s\n",
_tag_dyneither(_tmp408,sizeof(char),28))),_tag_dyneither(_tmp407,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 1;}}Cyc_fclose((struct Cyc___cycFILE*)
_tmp9D);return 0;};};}struct Cyc_List_List*Cyc_do_print(struct Cyc___cycFILE*
out_file,struct Cyc_List_List*tds);struct Cyc_List_List*Cyc_do_print(struct Cyc___cycFILE*
out_file,struct Cyc_List_List*tds){struct Cyc_Absynpp_Params _tmpA7=Cyc_tc_r?Cyc_Absynpp_cyc_params_r:
Cyc_Absynpp_c_params_r;_tmpA7.expand_typedefs=!Cyc_noexpand_r;_tmpA7.to_VC=Cyc_Cyclone_tovc_r;
_tmpA7.add_cyc_prefix=Cyc_add_cyc_namespace_r;_tmpA7.generate_line_directives=
Cyc_generate_line_directives_r;_tmpA7.print_full_evars=Cyc_print_full_evars_r;
_tmpA7.print_all_tvars=Cyc_print_all_tvars_r;_tmpA7.print_all_kinds=Cyc_print_all_kinds_r;
_tmpA7.print_all_effects=Cyc_print_all_effects_r;if(Cyc_inline_functions_r){
const char*_tmp40B;void*_tmp40A;(_tmp40A=0,Cyc_fprintf(out_file,((_tmp40B="#define _INLINE_FUNCTIONS\n",
_tag_dyneither(_tmp40B,sizeof(char),27))),_tag_dyneither(_tmp40A,sizeof(void*),0)));}
if((!Cyc_parseonly_r  && !Cyc_tc_r) && !Cyc_cf_r){if(!Cyc_nocyc_setjmp_r){if(Cyc_copy_internal_file(
Cyc_cyc_setjmp,out_file))return tds;}if(Cyc_copy_internal_file(Cyc_cyc_include,
out_file))return tds;}if(Cyc_pp_r){Cyc_Absynpp_set_params(& _tmpA7);Cyc_Absynpp_decllist2file(
tds,out_file);}else{Cyc_Absyndump_set_params(& _tmpA7);Cyc_Absyndump_dumpdecllist2file(
tds,out_file);}Cyc_fflush((struct Cyc___cycFILE*)out_file);return tds;}static
struct Cyc_List_List*Cyc_cfiles=0;static void Cyc_remove_cfiles();static void Cyc_remove_cfiles(){
if(!Cyc_save_c_r)for(0;Cyc_cfiles != 0;Cyc_cfiles=((struct Cyc_List_List*)
_check_null(Cyc_cfiles))->tl){Cyc_remove_file(*((struct _dyneither_ptr*)((struct
Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}static struct Cyc_List_List*Cyc_split_by_char(
struct _dyneither_ptr s,char c);static struct Cyc_List_List*Cyc_split_by_char(struct
_dyneither_ptr s,char c){if(s.curr == (_tag_dyneither(0,0,0)).curr)return 0;{struct
Cyc_List_List*_tmpAA=0;unsigned long _tmpAB=Cyc_strlen((struct _dyneither_ptr)s);
while(_tmpAB > 0){struct _dyneither_ptr _tmpAC=Cyc_strchr(s,c);if(_tmpAC.curr == (
_tag_dyneither(0,0,0)).curr){{struct _dyneither_ptr*_tmp40E;struct Cyc_List_List*
_tmp40D;_tmpAA=((_tmp40D=_cycalloc(sizeof(*_tmp40D)),((_tmp40D->hd=((_tmp40E=
_cycalloc(sizeof(*_tmp40E)),((_tmp40E[0]=s,_tmp40E)))),((_tmp40D->tl=_tmpAA,
_tmp40D))))));}break;}else{{struct _dyneither_ptr*_tmp411;struct Cyc_List_List*
_tmp410;_tmpAA=((_tmp410=_cycalloc(sizeof(*_tmp410)),((_tmp410->hd=((_tmp411=
_cycalloc(sizeof(*_tmp411)),((_tmp411[0]=(struct _dyneither_ptr)Cyc_substring((
struct _dyneither_ptr)s,0,(unsigned long)((((struct _dyneither_ptr)_tmpAC).curr - s.curr)
/ sizeof(char))),_tmp411)))),((_tmp410->tl=_tmpAA,_tmp410))))));}_tmpAB -=(((
struct _dyneither_ptr)_tmpAC).curr - s.curr)/ sizeof(char);s=_dyneither_ptr_plus(
_tmpAC,sizeof(char),1);}}return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmpAA);};}static int Cyc_file_exists(struct _dyneither_ptr file);
static int Cyc_file_exists(struct _dyneither_ptr file){struct Cyc___cycFILE*f=0;{
struct _handler_cons _tmpB1;_push_handler(& _tmpB1);{int _tmpB3=0;if(setjmp(_tmpB1.handler))
_tmpB3=1;if(!_tmpB3){{const char*_tmp412;f=(struct Cyc___cycFILE*)Cyc_file_open(
file,((_tmp412="r",_tag_dyneither(_tmp412,sizeof(char),2))));};_pop_handler();}
else{void*_tmpB2=(void*)_exn_thrown;void*_tmpB6=_tmpB2;_LL13:;_LL14: goto _LL12;
_LL15:;_LL16:(void)_throw(_tmpB6);_LL12:;}};}if(f == 0)return 0;else{Cyc_fclose((
struct Cyc___cycFILE*)f);return 1;}}static struct _dyneither_ptr Cyc_sprint_list(
struct Cyc_List_List*dirs);static struct _dyneither_ptr Cyc_sprint_list(struct Cyc_List_List*
dirs){const char*_tmp413;struct _dyneither_ptr tmp=(_tmp413="",_tag_dyneither(
_tmp413,sizeof(char),1));for(0;dirs != 0;dirs=dirs->tl){struct _dyneither_ptr
_tmpB7=*((struct _dyneither_ptr*)dirs->hd);if(_tmpB7.curr == (_tag_dyneither(0,0,0)).curr
 || Cyc_strlen((struct _dyneither_ptr)_tmpB7)== 0)continue;{const char*_tmp414;
_tmpB7=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmpB7,((
_tmp414=":",_tag_dyneither(_tmp414,sizeof(char),2))));}tmp=(struct _dyneither_ptr)
Cyc_strconcat((struct _dyneither_ptr)_tmpB7,(struct _dyneither_ptr)tmp);}return tmp;}
static struct _dyneither_ptr*Cyc_find(struct Cyc_List_List*dirs,struct
_dyneither_ptr file);static struct _dyneither_ptr*Cyc_find(struct Cyc_List_List*dirs,
struct _dyneither_ptr file){if(file.curr == (_tag_dyneither(0,0,0)).curr)return 0;
for(0;dirs != 0;dirs=dirs->tl){struct _dyneither_ptr _tmpBA=*((struct _dyneither_ptr*)
dirs->hd);if(_tmpBA.curr == (_tag_dyneither(0,0,0)).curr  || Cyc_strlen((struct
_dyneither_ptr)_tmpBA)== 0)continue;{struct _dyneither_ptr s=(struct _dyneither_ptr)
Cyc_Filename_concat(_tmpBA,file);if(Cyc_file_exists(s)){struct _dyneither_ptr*
_tmp415;return(_tmp415=_cycalloc(sizeof(*_tmp415)),((_tmp415[0]=s,_tmp415)));}};}
return 0;}static struct _dyneither_ptr Cyc_do_find(struct Cyc_List_List*dirs,struct
_dyneither_ptr file);static struct _dyneither_ptr Cyc_do_find(struct Cyc_List_List*
dirs,struct _dyneither_ptr file){struct _dyneither_ptr*_tmpBC=Cyc_find(dirs,file);
if(_tmpBC == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();{const char*_tmp41A;void*
_tmp419[2];struct Cyc_String_pa_struct _tmp418;struct Cyc_String_pa_struct _tmp417;(
_tmp417.tag=0,((_tmp417.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sprint_list(
dirs)),((_tmp418.tag=0,((_tmp418.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
file),((_tmp419[0]=& _tmp418,((_tmp419[1]=& _tmp417,Cyc_fprintf(Cyc_stderr,((
_tmp41A="Error: can't find internal compiler file %s in path %s\n",
_tag_dyneither(_tmp41A,sizeof(char),56))),_tag_dyneither(_tmp419,sizeof(void*),2)))))))))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Core_Failure_struct
_tmp42A;const char*_tmp429;void*_tmp428[2];struct Cyc_String_pa_struct _tmp427;
struct Cyc_String_pa_struct _tmp426;struct Cyc_Core_Failure_struct*_tmp425;(int)
_throw((void*)((_tmp425=_cycalloc(sizeof(*_tmp425)),((_tmp425[0]=((_tmp42A.tag=
Cyc_Core_Failure,((_tmp42A.f1=(struct _dyneither_ptr)((_tmp426.tag=0,((_tmp426.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sprint_list(dirs)),((_tmp427.tag=
0,((_tmp427.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp428[0]=&
_tmp427,((_tmp428[1]=& _tmp426,Cyc_aprintf(((_tmp429="Error: can't find internal compiler file %s in path %s\n",
_tag_dyneither(_tmp429,sizeof(char),56))),_tag_dyneither(_tmp428,sizeof(void*),2)))))))))))))),
_tmp42A)))),_tmp425)))));};}return*_tmpBC;}static struct Cyc_List_List*Cyc_add_subdir(
struct Cyc_List_List*dirs,struct _dyneither_ptr subdir);static struct Cyc_List_List*
Cyc_add_subdir(struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){struct Cyc_List_List*
_tmpC7=0;for(0;dirs != 0;dirs=dirs->tl){struct _dyneither_ptr*_tmp42D;struct Cyc_List_List*
_tmp42C;_tmpC7=((_tmp42C=_cycalloc(sizeof(*_tmp42C)),((_tmp42C->hd=((_tmp42D=
_cycalloc(sizeof(*_tmp42D)),((_tmp42D[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((
struct _dyneither_ptr*)dirs->hd),subdir),_tmp42D)))),((_tmp42C->tl=_tmpC7,_tmp42C))))));}
_tmpC7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpC7);
return _tmpC7;}static struct Cyc_List_List*Cyc_also_subdir(struct Cyc_List_List*dirs,
struct _dyneither_ptr subdir);static struct Cyc_List_List*Cyc_also_subdir(struct Cyc_List_List*
dirs,struct _dyneither_ptr subdir){struct Cyc_List_List*_tmpCA=0;for(0;dirs != 0;
dirs=dirs->tl){{struct Cyc_List_List*_tmp42E;_tmpCA=((_tmp42E=_cycalloc(sizeof(*
_tmp42E)),((_tmp42E->hd=(struct _dyneither_ptr*)dirs->hd,((_tmp42E->tl=_tmpCA,
_tmp42E))))));}{struct _dyneither_ptr*_tmp431;struct Cyc_List_List*_tmp430;_tmpCA=((
_tmp430=_cycalloc(sizeof(*_tmp430)),((_tmp430->hd=((_tmp431=_cycalloc(sizeof(*
_tmp431)),((_tmp431[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct
_dyneither_ptr*)dirs->hd),subdir),_tmp431)))),((_tmp430->tl=_tmpCA,_tmp430))))));};}
_tmpCA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpCA);
return _tmpCA;}static int Cyc_is_other_special(char c);static int Cyc_is_other_special(
char c){switch(c){case '\\': _LL17: goto _LL18;case '"': _LL18: goto _LL19;case ';': _LL19:
goto _LL1A;case '&': _LL1A: goto _LL1B;case '(': _LL1B: goto _LL1C;case ')': _LL1C: goto
_LL1D;case '|': _LL1D: goto _LL1E;case '^': _LL1E: goto _LL1F;case '<': _LL1F: goto _LL20;
case '>': _LL20: goto _LL21;case ' ': _LL21: goto _LL22;case '\n': _LL22: goto _LL23;case '\t':
_LL23: return 1;default: _LL24: return 0;}}static struct _dyneither_ptr Cyc_sh_escape_string(
struct _dyneither_ptr s);static void _tmp43A(unsigned int*_tmp439,unsigned int*
_tmp438,char**_tmp436){for(*_tmp439=0;*_tmp439 < *_tmp438;(*_tmp439)++){(*_tmp436)[*
_tmp439]='\000';}}static struct _dyneither_ptr Cyc_sh_escape_string(struct
_dyneither_ptr s){unsigned long _tmpCE=Cyc_strlen((struct _dyneither_ptr)s);int
_tmpCF=0;int _tmpD0=0;{int i=0;for(0;i < _tmpCE;++ i){char _tmpD1=*((const char*)
_check_dyneither_subscript(s,sizeof(char),i));if(_tmpD1 == '\'')++ _tmpCF;else{if(
Cyc_is_other_special(_tmpD1))++ _tmpD0;}}}if(_tmpCF == 0  && _tmpD0 == 0)return s;if(
_tmpCF == 0){struct _dyneither_ptr*_tmp434;struct _dyneither_ptr*_tmp433[3];return(
struct _dyneither_ptr)Cyc_strconcat_l(((_tmp433[2]=_init_dyneither_ptr(_cycalloc(
sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((_tmp433[1]=((_tmp434=
_cycalloc(sizeof(*_tmp434)),((_tmp434[0]=(struct _dyneither_ptr)s,_tmp434)))),((
_tmp433[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",
sizeof(char),2),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp433,sizeof(struct _dyneither_ptr*),3)))))))));}{unsigned long
_tmpD6=(_tmpCE + _tmpCF)+ _tmpD0;unsigned int _tmp439;unsigned int _tmp438;struct
_dyneither_ptr _tmp437;char*_tmp436;unsigned int _tmp435;struct _dyneither_ptr s2=(
_tmp435=_tmpD6 + 1,((_tmp436=(char*)_cycalloc_atomic(_check_times(sizeof(char),
_tmp435 + 1)),((_tmp437=_tag_dyneither(_tmp436,sizeof(char),_tmp435 + 1),((((
_tmp438=_tmp435,((_tmp43A(& _tmp439,& _tmp438,& _tmp436),_tmp436[_tmp438]=(char)0)))),
_tmp437)))))));int _tmpD7=0;int _tmpD8=0;for(0;_tmpD7 < _tmpCE;++ _tmpD7){char _tmpD9=*((
const char*)_check_dyneither_subscript(s,sizeof(char),_tmpD7));if(_tmpD9 == '\''
 || Cyc_is_other_special(_tmpD9)){char _tmp43D;char _tmp43C;struct _dyneither_ptr
_tmp43B;(_tmp43B=_dyneither_ptr_plus(s2,sizeof(char),_tmpD8 ++),((_tmp43C=*((char*)
_check_dyneither_subscript(_tmp43B,sizeof(char),0)),((_tmp43D='\\',((
_get_dyneither_size(_tmp43B,sizeof(char))== 1  && (_tmp43C == '\000'  && _tmp43D != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp43B.curr)=_tmp43D)))))));}{char _tmp440;char
_tmp43F;struct _dyneither_ptr _tmp43E;(_tmp43E=_dyneither_ptr_plus(s2,sizeof(char),
_tmpD8 ++),((_tmp43F=*((char*)_check_dyneither_subscript(_tmp43E,sizeof(char),0)),((
_tmp440=_tmpD9,((_get_dyneither_size(_tmp43E,sizeof(char))== 1  && (_tmp43F == '\000'
 && _tmp440 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp43E.curr)=_tmp440)))))));};}
return(struct _dyneither_ptr)s2;};}static struct _dyneither_ptr*Cyc_sh_escape_stringptr(
struct _dyneither_ptr*sp);static struct _dyneither_ptr*Cyc_sh_escape_stringptr(
struct _dyneither_ptr*sp){struct _dyneither_ptr*_tmp441;return(_tmp441=_cycalloc(
sizeof(*_tmp441)),((_tmp441[0]=Cyc_sh_escape_string(*sp),_tmp441)));}static void
Cyc_process_file(struct _dyneither_ptr filename);static void Cyc_process_file(struct
_dyneither_ptr filename){struct _dyneither_ptr _tmpE5=Cyc_make_base_filename(
filename,Cyc_output_file);const char*_tmp442;struct _dyneither_ptr _tmpE6=Cyc_strconcat((
struct _dyneither_ptr)_tmpE5,((_tmp442=".cyp",_tag_dyneither(_tmp442,sizeof(char),
5))));const char*_tmp443;struct _dyneither_ptr _tmpE7=Cyc_strconcat((struct
_dyneither_ptr)_tmpE5,((_tmp443=".cyci",_tag_dyneither(_tmp443,sizeof(char),6))));
const char*_tmp444;struct _dyneither_ptr _tmpE8=Cyc_strconcat((struct _dyneither_ptr)
_tmpE5,((_tmp444=".cycio",_tag_dyneither(_tmp444,sizeof(char),7))));const char*
_tmp445;struct _dyneither_ptr _tmpE9=Cyc_strconcat((struct _dyneither_ptr)_tmpE5,((
_tmp445=".c",_tag_dyneither(_tmp445,sizeof(char),3))));if(Cyc_v_r){const char*
_tmp449;void*_tmp448[1];struct Cyc_String_pa_struct _tmp447;(_tmp447.tag=0,((
_tmp447.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename),((_tmp448[0]=&
_tmp447,Cyc_fprintf(Cyc_stderr,((_tmp449="Compiling %s\n",_tag_dyneither(_tmp449,
sizeof(char),14))),_tag_dyneither(_tmp448,sizeof(void*),1)))))));}{const char*
_tmp44B;const char*_tmp44A;struct Cyc___cycFILE*f0=Cyc_try_file_open(filename,((
_tmp44A="r",_tag_dyneither(_tmp44A,sizeof(char),2))),((_tmp44B="input file",
_tag_dyneither(_tmp44B,sizeof(char),11))));if(Cyc_compile_failure  || !((
unsigned int)f0))return;Cyc_fclose((struct Cyc___cycFILE*)f0);{const char*_tmp452;
struct _dyneither_ptr*_tmp451;const char*_tmp450;struct Cyc_List_List*_tmp44F;
struct _dyneither_ptr _tmpED=Cyc_str_sepstr(((_tmp44F=_cycalloc(sizeof(*_tmp44F)),((
_tmp44F->hd=((_tmp451=_cycalloc(sizeof(*_tmp451)),((_tmp451[0]=(struct
_dyneither_ptr)((_tmp450="",_tag_dyneither(_tmp450,sizeof(char),1))),_tmp451)))),((
_tmp44F->tl=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs)),
_tmp44F)))))),((_tmp452=" ",_tag_dyneither(_tmp452,sizeof(char),2))));struct
_dyneither_ptr def_inc_path=(struct _dyneither_ptr)Cstring_to_string(Cdef_inc_path);
struct Cyc_List_List*_tmpEE=Cyc_add_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);{
const char*_tmp453;_tmpEE=Cyc_add_subdir(_tmpEE,((_tmp453="include",
_tag_dyneither(_tmp453,sizeof(char),8))));}if(Cyc_strlen((struct _dyneither_ptr)
def_inc_path)> 0){struct _dyneither_ptr*_tmp456;struct Cyc_List_List*_tmp455;
_tmpEE=((_tmp455=_cycalloc(sizeof(*_tmp455)),((_tmp455->hd=((_tmp456=_cycalloc(
sizeof(*_tmp456)),((_tmp456[0]=def_inc_path,_tmp456)))),((_tmp455->tl=_tmpEE,
_tmp455))))));}{char*_tmpF2=getenv((const char*)"CYCLONE_INCLUDE_PATH");if((char*)
_tmpF2 != 0){char*_tmp457;_tmpEE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(Cyc_split_by_char(((_tmp457=_tmpF2,
_tag_dyneither(_tmp457,sizeof(char),_get_zero_arr_size_char(_tmp457,1)))),':'),
_tmpEE);}{const char*_tmp45E;struct _dyneither_ptr*_tmp45D;const char*_tmp45C;
struct Cyc_List_List*_tmp45B;struct _dyneither_ptr stdinc_string=(struct
_dyneither_ptr)Cyc_str_sepstr(((_tmp45B=_cycalloc(sizeof(*_tmp45B)),((_tmp45B->hd=((
_tmp45D=_cycalloc(sizeof(*_tmp45D)),((_tmp45D[0]=(struct _dyneither_ptr)((_tmp45C="",
_tag_dyneither(_tmp45C,sizeof(char),1))),_tmp45D)))),((_tmp45B->tl=((struct Cyc_List_List*(*)(
struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,_tmpEE),_tmp45B)))))),((_tmp45E=" -I",_tag_dyneither(
_tmp45E,sizeof(char),4))));struct _dyneither_ptr ofile_string;if(Cyc_stop_after_cpp_r){
if(Cyc_output_file != 0){const char*_tmp462;void*_tmp461[1];struct Cyc_String_pa_struct
_tmp460;ofile_string=(struct _dyneither_ptr)((_tmp460.tag=0,((_tmp460.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_output_file))),((
_tmp461[0]=& _tmp460,Cyc_aprintf(((_tmp462=" > %s",_tag_dyneither(_tmp462,sizeof(
char),6))),_tag_dyneither(_tmp461,sizeof(void*),1))))))));}else{const char*
_tmp463;ofile_string=((_tmp463="",_tag_dyneither(_tmp463,sizeof(char),1)));}}
else{const char*_tmp467;void*_tmp466[1];struct Cyc_String_pa_struct _tmp465;
ofile_string=(struct _dyneither_ptr)((_tmp465.tag=0,((_tmp465.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string((struct _dyneither_ptr)
_tmpE6)),((_tmp466[0]=& _tmp465,Cyc_aprintf(((_tmp467=" > %s",_tag_dyneither(
_tmp467,sizeof(char),6))),_tag_dyneither(_tmp466,sizeof(void*),1))))))));}{
struct _dyneither_ptr fixup_string;if(Cyc_produce_dependencies){if(Cyc_dependencies_target
== 0){const char*_tmp468;fixup_string=((_tmp468=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'",
_tag_dyneither(_tmp468,sizeof(char),35)));}else{const char*_tmp46C;void*_tmp46B[1];
struct Cyc_String_pa_struct _tmp46A;fixup_string=(struct _dyneither_ptr)((_tmp46A.tag=
0,((_tmp46A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct
_dyneither_ptr*)_check_null(Cyc_dependencies_target))),((_tmp46B[0]=& _tmp46A,Cyc_aprintf(((
_tmp46C=" | sed 's/^.*\\.cyc\\.o:/%s:/'",_tag_dyneither(_tmp46C,sizeof(char),29))),
_tag_dyneither(_tmp46B,sizeof(void*),1))))))));}}else{const char*_tmp46D;
fixup_string=((_tmp46D="",_tag_dyneither(_tmp46D,sizeof(char),1)));}{const char*
_tmp476;void*_tmp475[6];struct Cyc_String_pa_struct _tmp474;struct Cyc_String_pa_struct
_tmp473;struct Cyc_String_pa_struct _tmp472;struct Cyc_String_pa_struct _tmp471;
struct Cyc_String_pa_struct _tmp470;struct Cyc_String_pa_struct _tmp46F;struct
_dyneither_ptr _tmp100=(_tmp46F.tag=0,((_tmp46F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)ofile_string),((_tmp470.tag=0,((_tmp470.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)fixup_string),((_tmp471.tag=0,((_tmp471.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(filename)),((_tmp472.tag=
0,((_tmp472.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdinc_string),((
_tmp473.tag=0,((_tmp473.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpED),((
_tmp474.tag=0,((_tmp474.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cpp),((
_tmp475[0]=& _tmp474,((_tmp475[1]=& _tmp473,((_tmp475[2]=& _tmp472,((_tmp475[3]=&
_tmp471,((_tmp475[4]=& _tmp470,((_tmp475[5]=& _tmp46F,Cyc_aprintf(((_tmp476="%s %s%s %s%s%s",
_tag_dyneither(_tmp476,sizeof(char),15))),_tag_dyneither(_tmp475,sizeof(void*),6)))))))))))))))))))))))))))))))))))));
if(Cyc_v_r){const char*_tmp47A;void*_tmp479[1];struct Cyc_String_pa_struct _tmp478;(
_tmp478.tag=0,((_tmp478.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp100),((
_tmp479[0]=& _tmp478,Cyc_fprintf(Cyc_stderr,((_tmp47A="%s\n",_tag_dyneither(
_tmp47A,sizeof(char),4))),_tag_dyneither(_tmp479,sizeof(void*),1)))))));}if(
system((const char*)_untag_dyneither_ptr(_tmp100,sizeof(char),1))!= 0){Cyc_compile_failure=
1;{const char*_tmp47D;void*_tmp47C;(_tmp47C=0,Cyc_fprintf(Cyc_stderr,((_tmp47D="\nError: preprocessing\n",
_tag_dyneither(_tmp47D,sizeof(char),23))),_tag_dyneither(_tmp47C,sizeof(void*),0)));}
return;}if(Cyc_stop_after_cpp_r)return;Cyc_Position_reset_position((struct
_dyneither_ptr)_tmpE6);{const char*_tmp47F;const char*_tmp47E;struct Cyc___cycFILE*
in_file=Cyc_try_file_open((struct _dyneither_ptr)_tmpE6,((_tmp47E="r",
_tag_dyneither(_tmp47E,sizeof(char),2))),((_tmp47F="file",_tag_dyneither(_tmp47F,
sizeof(char),5))));if(Cyc_compile_failure)return;{struct Cyc_List_List*tds=0;{
struct _handler_cons _tmp106;_push_handler(& _tmp106);{int _tmp108=0;if(setjmp(
_tmp106.handler))_tmp108=1;if(!_tmp108){{const char*_tmp480;tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,
struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp480="parsing",_tag_dyneither(_tmp480,sizeof(char),8))),Cyc_do_parse,(struct
Cyc___cycFILE*)_check_null(in_file),tds);};_pop_handler();}else{void*_tmp107=(
void*)_exn_thrown;void*_tmp10B=_tmp107;_LL27:;_LL28: Cyc_file_close((struct Cyc___cycFILE*)
_check_null(in_file));Cyc_remove_file((struct _dyneither_ptr)_tmpE6);(int)_throw(
_tmp10B);_LL29:;_LL2A:(void)_throw(_tmp10B);_LL26:;}};}Cyc_file_close((struct Cyc___cycFILE*)
in_file);if(Cyc_compile_failure){Cyc_remove_file((struct _dyneither_ptr)_tmpE6);
return;}if(Cyc_Absyn_porting_c_code){Cyc_Port_port(tds);return;}{struct
_RegionHandle _tmp10C=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmp10C;
_push_region(tc_rgn);{struct Cyc_Tcenv_Tenv*_tmp10D=Cyc_Tcenv_tc_init(tc_rgn);if(
Cyc_parseonly_r)goto PRINTC;{struct _tuple20 _tmp481;struct _tuple20 _tmp10E=(_tmp481.f1=
tc_rgn,((_tmp481.f2=_tmp10D,_tmp481)));{const char*_tmp482;tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple20*,struct
Cyc_List_List*),struct _tuple20*env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp482="type checking",_tag_dyneither(_tmp482,sizeof(char),14))),Cyc_do_typecheck,&
_tmp10E,tds);}if(Cyc_compile_failure){Cyc_remove_file((struct _dyneither_ptr)
_tmpE6);_npop_handler(0);return;}if(Cyc_tc_r)goto PRINTC;{const char*_tmp483;tds=((
struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(
int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp483="control-flow checking",_tag_dyneither(_tmp483,sizeof(char),22))),Cyc_do_cfcheck,
1,tds);}if(Cyc_compile_failure){Cyc_remove_file((struct _dyneither_ptr)_tmpE6);
_npop_handler(0);return;}if(Cyc_ic_r){struct Cyc___cycFILE*inter_file=Cyc_fopen((
const char*)((char*)_untag_dyneither_ptr(_tmpE7,sizeof(char),1)),(const char*)"r");
const char*_tmp485;const char*_tmp484;struct Cyc___cycFILE*inter_objfile=Cyc_try_file_open((
struct _dyneither_ptr)_tmpE8,((_tmp484="w",_tag_dyneither(_tmp484,sizeof(char),2))),((
_tmp485="interface object file",_tag_dyneither(_tmp485,sizeof(char),22))));if(
inter_objfile == 0){Cyc_compile_failure=1;_npop_handler(0);return;}Cyc_Position_reset_position((
struct _dyneither_ptr)_tmpE7);{struct _tuple21 _tmp486;struct _tuple21 _tmp111=(
_tmp486.f1=_tmp10D,((_tmp486.f2=inter_file,((_tmp486.f3=(struct Cyc___cycFILE*)
inter_objfile,_tmp486)))));{const char*_tmp487;tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple21*,struct
Cyc_List_List*),struct _tuple21*env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp487="interface checking",_tag_dyneither(_tmp487,sizeof(char),19))),Cyc_do_interface,&
_tmp111,tds);}if(inter_file != 0)Cyc_file_close((struct Cyc___cycFILE*)inter_file);
Cyc_file_close((struct Cyc___cycFILE*)inter_objfile);};}};}if(Cyc_cf_r)goto PRINTC;{
const char*_tmp488;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,
struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*
tds))Cyc_do_stage)(((_tmp488="translation to C",_tag_dyneither(_tmp488,sizeof(
char),17))),Cyc_do_translate,1,tds);}if(Cyc_compile_failure){Cyc_remove_file((
struct _dyneither_ptr)_tmpE6);_npop_handler(0);return;}if(!Cyc_compile_failure)
Cyc_Tcutil_flush_warnings();Cyc_remove_file((struct _dyneither_ptr)_tmpE6);if(Cyc_compile_failure){
_npop_handler(0);return;}if(Cyc_Cyclone_tovc_r  || Cyc_elim_se_r){const char*
_tmp489;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*
f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp489="post-pass to VC",_tag_dyneither(_tmp489,sizeof(char),16))),Cyc_do_tovc,
1,tds);}if(Cyc_compile_failure){_npop_handler(0);return;}PRINTC: {struct Cyc___cycFILE*
out_file;if((Cyc_parseonly_r  || Cyc_tc_r) || Cyc_cf_r){if(Cyc_output_file != 0){
const char*_tmp48B;const char*_tmp48A;out_file=Cyc_try_file_open(*((struct
_dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp48A="w",_tag_dyneither(
_tmp48A,sizeof(char),2))),((_tmp48B="output file",_tag_dyneither(_tmp48B,sizeof(
char),12))));}else{out_file=(struct Cyc___cycFILE*)Cyc_stdout;}}else{if(Cyc_toc_r
 && Cyc_output_file != 0){const char*_tmp48D;const char*_tmp48C;out_file=Cyc_try_file_open(*((
struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp48C="w",_tag_dyneither(
_tmp48C,sizeof(char),2))),((_tmp48D="output file",_tag_dyneither(_tmp48D,sizeof(
char),12))));}else{const char*_tmp48F;const char*_tmp48E;out_file=Cyc_try_file_open((
struct _dyneither_ptr)_tmpE9,((_tmp48E="w",_tag_dyneither(_tmp48E,sizeof(char),2))),((
_tmp48F="output file",_tag_dyneither(_tmp48F,sizeof(char),12))));}}if(Cyc_compile_failure
 || !((unsigned int)out_file)){_npop_handler(0);return;}{struct _handler_cons
_tmp11F;_push_handler(& _tmp11F);{int _tmp121=0;if(setjmp(_tmp11F.handler))_tmp121=
1;if(!_tmp121){if(!Cyc_noprint_r){const char*_tmp490;tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,
struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp490="printing",_tag_dyneither(_tmp490,sizeof(char),9))),Cyc_do_print,(struct
Cyc___cycFILE*)out_file,tds);};_pop_handler();}else{void*_tmp120=(void*)
_exn_thrown;void*_tmp124=_tmp120;_LL2C:;_LL2D: Cyc_compile_failure=1;Cyc_file_close((
struct Cyc___cycFILE*)out_file);{struct _dyneither_ptr*_tmp493;struct Cyc_List_List*
_tmp492;Cyc_cfiles=((_tmp492=_cycalloc(sizeof(*_tmp492)),((_tmp492->hd=((_tmp493=
_cycalloc(sizeof(*_tmp493)),((_tmp493[0]=(struct _dyneither_ptr)_tmpE9,_tmp493)))),((
_tmp492->tl=Cyc_cfiles,_tmp492))))));}(int)_throw(_tmp124);_LL2E:;_LL2F:(void)
_throw(_tmp124);_LL2B:;}};}Cyc_file_close((struct Cyc___cycFILE*)out_file);{
struct _dyneither_ptr*_tmp496;struct Cyc_List_List*_tmp495;Cyc_cfiles=((_tmp495=
_cycalloc(sizeof(*_tmp495)),((_tmp495->hd=((_tmp496=_cycalloc(sizeof(*_tmp496)),((
_tmp496[0]=(struct _dyneither_ptr)_tmpE9,_tmp496)))),((_tmp495->tl=Cyc_cfiles,
_tmp495))))));};};_pop_region(tc_rgn);};};};};};};};};};}static char _tmp141[8]="<final>";
static struct _dyneither_ptr Cyc_final_str={_tmp141,_tmp141,_tmp141 + 8};static
struct _dyneither_ptr*Cyc_final_strptr=& Cyc_final_str;static struct Cyc_Interface_I*
Cyc_read_cycio(struct _dyneither_ptr*n);static struct Cyc_Interface_I*Cyc_read_cycio(
struct _dyneither_ptr*n){if(n == (struct _dyneither_ptr*)Cyc_final_strptr)return Cyc_Interface_final();{
struct _dyneither_ptr basename;{struct _handler_cons _tmp142;_push_handler(& _tmp142);{
int _tmp144=0;if(setjmp(_tmp142.handler))_tmp144=1;if(!_tmp144){basename=(struct
_dyneither_ptr)Cyc_Filename_chop_extension(*n);;_pop_handler();}else{void*
_tmp143=(void*)_exn_thrown;void*_tmp146=_tmp143;_LL31: {struct Cyc_Core_Invalid_argument_struct*
_tmp147=(struct Cyc_Core_Invalid_argument_struct*)_tmp146;if(_tmp147->tag != Cyc_Core_Invalid_argument)
goto _LL33;}_LL32: basename=*n;goto _LL30;_LL33:;_LL34:(void)_throw(_tmp146);_LL30:;}};}{
const char*_tmp497;struct _dyneither_ptr _tmp148=Cyc_strconcat((struct
_dyneither_ptr)basename,((_tmp497=".cycio",_tag_dyneither(_tmp497,sizeof(char),7))));
const char*_tmp499;const char*_tmp498;struct Cyc___cycFILE*_tmp149=Cyc_try_file_open((
struct _dyneither_ptr)_tmp148,((_tmp498="rb",_tag_dyneither(_tmp498,sizeof(char),
3))),((_tmp499="interface object file",_tag_dyneither(_tmp499,sizeof(char),22))));
if(_tmp149 == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();exit(1);}Cyc_Position_reset_position((
struct _dyneither_ptr)_tmp148);{struct Cyc_Interface_I*_tmp14A=Cyc_Interface_load((
struct Cyc___cycFILE*)_tmp149);Cyc_file_close((struct Cyc___cycFILE*)_tmp149);
return _tmp14A;};};};}static int Cyc_is_cfile(struct _dyneither_ptr*n);static int Cyc_is_cfile(
struct _dyneither_ptr*n){return*((const char*)_check_dyneither_subscript(*n,
sizeof(char),0))!= '-';}void GC_blacklist_warn_clear();struct _tuple22{struct
_dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
int Cyc_main(int argc,struct _dyneither_ptr argv);static void _tmp49F(unsigned int*
_tmp49E,unsigned int*_tmp49D,int**_tmp49B){for(*_tmp49E=0;*_tmp49E < *_tmp49D;(*
_tmp49E)++){(*_tmp49B)[*_tmp49E]=0;}}static void _tmp4A7(unsigned int*_tmp4A6,
unsigned int*_tmp4A5,struct _dyneither_ptr**_tmp4A3){for(*_tmp4A6=0;*_tmp4A6 < *
_tmp4A5;(*_tmp4A6)++){(*_tmp4A3)[*_tmp4A6]=(struct _dyneither_ptr)_tag_dyneither(
0,0,0);}}static void _tmp4AD(unsigned int*_tmp4AC,unsigned int*_tmp4AB,struct
_dyneither_ptr**_tmp4A9){for(*_tmp4AC=0;*_tmp4AC < *_tmp4AB;(*_tmp4AC)++){(*
_tmp4A9)[*_tmp4AC]=(struct _dyneither_ptr)_tag_dyneither(0,0,0);}}int Cyc_main(int
argc,struct _dyneither_ptr argv){GC_blacklist_warn_clear();{struct Cyc_List_List*
cyclone_arch_path;struct _dyneither_ptr def_lib_path;struct _dyneither_ptr comp=(
struct _dyneither_ptr)Cstring_to_string(Ccomp);struct _RegionHandle _tmp14E=
_new_region("r");struct _RegionHandle*r=& _tmp14E;_push_region(r);{unsigned int
_tmp49E;unsigned int _tmp49D;struct _dyneither_ptr _tmp49C;int*_tmp49B;unsigned int
_tmp49A;struct _dyneither_ptr _tmp14F=(_tmp49A=(unsigned int)argc,((_tmp49B=(int*)
_cycalloc_atomic(_check_times(sizeof(int),_tmp49A)),((_tmp49C=_tag_dyneither(
_tmp49B,sizeof(int),_tmp49A),((((_tmp49D=_tmp49A,_tmp49F(& _tmp49E,& _tmp49D,&
_tmp49B))),_tmp49C)))))));int _tmp150=0;int i;int j;int k;for(i=1;i < argc;++ i){const
char*_tmp4A0;if(Cyc_strncmp(((_tmp4A0="-B",_tag_dyneither(_tmp4A0,sizeof(char),3))),(
struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,
sizeof(struct _dyneither_ptr),i)),2)== 0){*((int*)_check_dyneither_subscript(
_tmp14F,sizeof(int),i))=1;++ _tmp150;}else{const char*_tmp4A1;if(Cyc_strcmp(((
_tmp4A1="-b",_tag_dyneither(_tmp4A1,sizeof(char),3))),(struct _dyneither_ptr)*((
struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct
_dyneither_ptr),i)))== 0){*((int*)_check_dyneither_subscript(_tmp14F,sizeof(int),
i))=1;++ _tmp150;++ i;if(i >= argc)break;*((int*)_check_dyneither_subscript(_tmp14F,
sizeof(int),i))=1;++ _tmp150;}}}{unsigned int _tmp4A6;unsigned int _tmp4A5;struct
_dyneither_ptr _tmp4A4;struct _dyneither_ptr*_tmp4A3;unsigned int _tmp4A2;struct
_dyneither_ptr _tmp153=(_tmp4A2=(unsigned int)(_tmp150 + 1),((_tmp4A3=(struct
_dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp4A2)),((
_tmp4A4=_tag_dyneither(_tmp4A3,sizeof(struct _dyneither_ptr),_tmp4A2),((((_tmp4A5=
_tmp4A2,_tmp4A7(& _tmp4A6,& _tmp4A5,& _tmp4A3))),_tmp4A4)))))));unsigned int _tmp4AC;
unsigned int _tmp4AB;struct _dyneither_ptr _tmp4AA;struct _dyneither_ptr*_tmp4A9;
unsigned int _tmp4A8;struct _dyneither_ptr _tmp154=(_tmp4A8=(unsigned int)(argc - 
_tmp150),((_tmp4A9=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct
_dyneither_ptr),_tmp4A8)),((_tmp4AA=_tag_dyneither(_tmp4A9,sizeof(struct
_dyneither_ptr),_tmp4A8),((((_tmp4AB=_tmp4A8,_tmp4AD(& _tmp4AC,& _tmp4AB,& _tmp4A9))),
_tmp4AA)))))));*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp153,
sizeof(struct _dyneither_ptr),0))=(*((struct _dyneither_ptr*)
_check_dyneither_subscript(_tmp154,sizeof(struct _dyneither_ptr),0))=*((struct
_dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),0)));
for(i=(j=(k=1));i < argc;++ i){if(*((int*)_check_dyneither_subscript(_tmp14F,
sizeof(int),i)))*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp153,
sizeof(struct _dyneither_ptr),j ++))=*((struct _dyneither_ptr*)
_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));else{*((struct
_dyneither_ptr*)_check_dyneither_subscript(_tmp154,sizeof(struct _dyneither_ptr),
k ++))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct
_dyneither_ptr),i));}}{struct _tuple22*_tmp92E;const char*_tmp92D;const char*
_tmp92C;struct Cyc_Arg_Set_spec_struct _tmp92B;struct Cyc_Arg_Set_spec_struct*
_tmp92A;const char*_tmp929;struct _tuple22*_tmp928;const char*_tmp927;const char*
_tmp926;struct Cyc_Arg_Unit_spec_struct _tmp925;struct Cyc_Arg_Unit_spec_struct*
_tmp924;const char*_tmp923;struct _tuple22*_tmp922;const char*_tmp921;const char*
_tmp920;struct Cyc_Arg_String_spec_struct _tmp91F;struct Cyc_Arg_String_spec_struct*
_tmp91E;const char*_tmp91D;struct _tuple22*_tmp91C;const char*_tmp91B;const char*
_tmp91A;struct Cyc_Arg_Flag_spec_struct _tmp919;struct Cyc_Arg_Flag_spec_struct*
_tmp918;const char*_tmp917;struct _tuple22*_tmp916;const char*_tmp915;const char*
_tmp914;struct Cyc_Arg_Flag_spec_struct _tmp913;struct Cyc_Arg_Flag_spec_struct*
_tmp912;const char*_tmp911;struct _tuple22*_tmp910;const char*_tmp90F;const char*
_tmp90E;struct Cyc_Arg_Flag_spec_struct _tmp90D;struct Cyc_Arg_Flag_spec_struct*
_tmp90C;const char*_tmp90B;struct _tuple22*_tmp90A;const char*_tmp909;const char*
_tmp908;struct Cyc_Arg_Flag_spec_struct _tmp907;struct Cyc_Arg_Flag_spec_struct*
_tmp906;const char*_tmp905;struct _tuple22*_tmp904;const char*_tmp903;const char*
_tmp902;struct Cyc_Arg_Flag_spec_struct _tmp901;struct Cyc_Arg_Flag_spec_struct*
_tmp900;const char*_tmp8FF;struct _tuple22*_tmp8FE;const char*_tmp8FD;const char*
_tmp8FC;struct Cyc_Arg_Flag_spec_struct _tmp8FB;struct Cyc_Arg_Flag_spec_struct*
_tmp8FA;const char*_tmp8F9;struct _tuple22*_tmp8F8;const char*_tmp8F7;const char*
_tmp8F6;struct Cyc_Arg_Unit_spec_struct _tmp8F5;struct Cyc_Arg_Unit_spec_struct*
_tmp8F4;const char*_tmp8F3;struct _tuple22*_tmp8F2;const char*_tmp8F1;const char*
_tmp8F0;struct Cyc_Arg_String_spec_struct _tmp8EF;struct Cyc_Arg_String_spec_struct*
_tmp8EE;const char*_tmp8ED;struct _tuple22*_tmp8EC;const char*_tmp8EB;const char*
_tmp8EA;struct Cyc_Arg_Flag_spec_struct _tmp8E9;struct Cyc_Arg_Flag_spec_struct*
_tmp8E8;const char*_tmp8E7;struct _tuple22*_tmp8E6;const char*_tmp8E5;const char*
_tmp8E4;struct Cyc_Arg_Flag_spec_struct _tmp8E3;struct Cyc_Arg_Flag_spec_struct*
_tmp8E2;const char*_tmp8E1;struct _tuple22*_tmp8E0;const char*_tmp8DF;const char*
_tmp8DE;struct Cyc_Arg_Flag_spec_struct _tmp8DD;struct Cyc_Arg_Flag_spec_struct*
_tmp8DC;const char*_tmp8DB;struct _tuple22*_tmp8DA;const char*_tmp8D9;const char*
_tmp8D8;struct Cyc_Arg_Flag_spec_struct _tmp8D7;struct Cyc_Arg_Flag_spec_struct*
_tmp8D6;const char*_tmp8D5;struct _tuple22*_tmp8D4;const char*_tmp8D3;const char*
_tmp8D2;struct Cyc_Arg_Flag_spec_struct _tmp8D1;struct Cyc_Arg_Flag_spec_struct*
_tmp8D0;const char*_tmp8CF;struct _tuple22*_tmp8CE;const char*_tmp8CD;const char*
_tmp8CC;struct Cyc_Arg_Flag_spec_struct _tmp8CB;struct Cyc_Arg_Flag_spec_struct*
_tmp8CA;const char*_tmp8C9;struct _tuple22*_tmp8C8;const char*_tmp8C7;const char*
_tmp8C6;struct Cyc_Arg_Flag_spec_struct _tmp8C5;struct Cyc_Arg_Flag_spec_struct*
_tmp8C4;const char*_tmp8C3;struct _tuple22*_tmp8C2;const char*_tmp8C1;const char*
_tmp8C0;struct Cyc_Arg_Flag_spec_struct _tmp8BF;struct Cyc_Arg_Flag_spec_struct*
_tmp8BE;const char*_tmp8BD;struct _tuple22*_tmp8BC;const char*_tmp8BB;const char*
_tmp8BA;struct Cyc_Arg_Unit_spec_struct _tmp8B9;struct Cyc_Arg_Unit_spec_struct*
_tmp8B8;const char*_tmp8B7;struct _tuple22*_tmp8B6;const char*_tmp8B5;const char*
_tmp8B4;struct Cyc_Arg_Unit_spec_struct _tmp8B3;struct Cyc_Arg_Unit_spec_struct*
_tmp8B2;const char*_tmp8B1;struct _tuple22*_tmp8B0;const char*_tmp8AF;const char*
_tmp8AE;struct Cyc_Arg_Unit_spec_struct _tmp8AD;struct Cyc_Arg_Unit_spec_struct*
_tmp8AC;const char*_tmp8AB;struct _tuple22*_tmp8AA;const char*_tmp8A9;const char*
_tmp8A8;struct Cyc_Arg_Unit_spec_struct _tmp8A7;struct Cyc_Arg_Unit_spec_struct*
_tmp8A6;const char*_tmp8A5;struct _tuple22*_tmp8A4;const char*_tmp8A3;const char*
_tmp8A2;struct Cyc_Arg_Flag_spec_struct _tmp8A1;struct Cyc_Arg_Flag_spec_struct*
_tmp8A0;const char*_tmp89F;struct _tuple22*_tmp89E;const char*_tmp89D;const char*
_tmp89C;struct Cyc_Arg_String_spec_struct _tmp89B;struct Cyc_Arg_String_spec_struct*
_tmp89A;const char*_tmp899;struct _tuple22*_tmp898;const char*_tmp897;const char*
_tmp896;struct Cyc_Arg_String_spec_struct _tmp895;struct Cyc_Arg_String_spec_struct*
_tmp894;const char*_tmp893;struct _tuple22*_tmp892;const char*_tmp891;const char*
_tmp890;struct Cyc_Arg_Unit_spec_struct _tmp88F;struct Cyc_Arg_Unit_spec_struct*
_tmp88E;const char*_tmp88D;struct _tuple22*_tmp88C;const char*_tmp88B;const char*
_tmp88A;struct Cyc_Arg_Set_spec_struct _tmp889;struct Cyc_Arg_Set_spec_struct*
_tmp888;const char*_tmp887;struct _tuple22*_tmp886;const char*_tmp885;const char*
_tmp884;struct Cyc_Arg_Set_spec_struct _tmp883;struct Cyc_Arg_Set_spec_struct*
_tmp882;const char*_tmp881;struct _tuple22*_tmp880;const char*_tmp87F;const char*
_tmp87E;struct Cyc_Arg_Set_spec_struct _tmp87D;struct Cyc_Arg_Set_spec_struct*
_tmp87C;const char*_tmp87B;struct _tuple22*_tmp87A;const char*_tmp879;const char*
_tmp878;struct Cyc_Arg_Set_spec_struct _tmp877;struct Cyc_Arg_Set_spec_struct*
_tmp876;const char*_tmp875;struct _tuple22*_tmp874;const char*_tmp873;const char*
_tmp872;struct Cyc_Arg_Set_spec_struct _tmp871;struct Cyc_Arg_Set_spec_struct*
_tmp870;const char*_tmp86F;struct _tuple22*_tmp86E;const char*_tmp86D;const char*
_tmp86C;struct Cyc_Arg_Set_spec_struct _tmp86B;struct Cyc_Arg_Set_spec_struct*
_tmp86A;const char*_tmp869;struct _tuple22*_tmp868;const char*_tmp867;const char*
_tmp866;struct Cyc_Arg_Set_spec_struct _tmp865;struct Cyc_Arg_Set_spec_struct*
_tmp864;const char*_tmp863;struct _tuple22*_tmp862;const char*_tmp861;const char*
_tmp860;struct Cyc_Arg_Set_spec_struct _tmp85F;struct Cyc_Arg_Set_spec_struct*
_tmp85E;const char*_tmp85D;struct _tuple22*_tmp85C;const char*_tmp85B;const char*
_tmp85A;struct Cyc_Arg_Clear_spec_struct _tmp859;struct Cyc_Arg_Clear_spec_struct*
_tmp858;const char*_tmp857;struct _tuple22*_tmp856;const char*_tmp855;const char*
_tmp854;struct Cyc_Arg_Set_spec_struct _tmp853;struct Cyc_Arg_Set_spec_struct*
_tmp852;const char*_tmp851;struct _tuple22*_tmp850;const char*_tmp84F;const char*
_tmp84E;struct Cyc_Arg_Unit_spec_struct _tmp84D;struct Cyc_Arg_Unit_spec_struct*
_tmp84C;const char*_tmp84B;struct _tuple22*_tmp84A;const char*_tmp849;const char*
_tmp848;struct Cyc_Arg_String_spec_struct _tmp847;struct Cyc_Arg_String_spec_struct*
_tmp846;const char*_tmp845;struct _tuple22*_tmp844;const char*_tmp843;const char*
_tmp842;struct Cyc_Arg_Unit_spec_struct _tmp841;struct Cyc_Arg_Unit_spec_struct*
_tmp840;const char*_tmp83F;struct _tuple22*_tmp83E;const char*_tmp83D;const char*
_tmp83C;struct Cyc_Arg_Set_spec_struct _tmp83B;struct Cyc_Arg_Set_spec_struct*
_tmp83A;const char*_tmp839;struct _tuple22*_tmp838;const char*_tmp837;const char*
_tmp836;struct Cyc_Arg_Unit_spec_struct _tmp835;struct Cyc_Arg_Unit_spec_struct*
_tmp834;const char*_tmp833;struct _tuple22*_tmp832;const char*_tmp831;const char*
_tmp830;struct Cyc_Arg_Unit_spec_struct _tmp82F;struct Cyc_Arg_Unit_spec_struct*
_tmp82E;const char*_tmp82D;struct _tuple22*_tmp82C;const char*_tmp82B;const char*
_tmp82A;struct Cyc_Arg_Unit_spec_struct _tmp829;struct Cyc_Arg_Unit_spec_struct*
_tmp828;const char*_tmp827;struct _tuple22*_tmp826;const char*_tmp825;const char*
_tmp824;struct Cyc_Arg_Unit_spec_struct _tmp823;struct Cyc_Arg_Unit_spec_struct*
_tmp822;const char*_tmp821;struct _tuple22*_tmp820;const char*_tmp81F;const char*
_tmp81E;struct Cyc_Arg_Unit_spec_struct _tmp81D;struct Cyc_Arg_Unit_spec_struct*
_tmp81C;const char*_tmp81B;struct _tuple22*_tmp81A;const char*_tmp819;const char*
_tmp818;struct Cyc_Arg_String_spec_struct _tmp817;struct Cyc_Arg_String_spec_struct*
_tmp816;const char*_tmp815;struct _tuple22*_tmp814;const char*_tmp813;const char*
_tmp812;struct Cyc_Arg_Unit_spec_struct _tmp811;struct Cyc_Arg_Unit_spec_struct*
_tmp810;const char*_tmp80F;struct _tuple22*_tmp80E;const char*_tmp80D;const char*
_tmp80C;struct Cyc_Arg_Unit_spec_struct _tmp80B;struct Cyc_Arg_Unit_spec_struct*
_tmp80A;const char*_tmp809;struct _tuple22*_tmp808;const char*_tmp807;const char*
_tmp806;struct Cyc_Arg_Set_spec_struct _tmp805;struct Cyc_Arg_Set_spec_struct*
_tmp804;const char*_tmp803;struct _tuple22*_tmp802;const char*_tmp801;const char*
_tmp800;struct Cyc_Arg_Set_spec_struct _tmp7FF;struct Cyc_Arg_Set_spec_struct*
_tmp7FE;const char*_tmp7FD;struct _tuple22*_tmp7FC;const char*_tmp7FB;const char*
_tmp7FA;struct Cyc_Arg_Set_spec_struct _tmp7F9;struct Cyc_Arg_Set_spec_struct*
_tmp7F8;const char*_tmp7F7;struct _tuple22*_tmp7F6;const char*_tmp7F5;const char*
_tmp7F4;struct Cyc_Arg_Set_spec_struct _tmp7F3;struct Cyc_Arg_Set_spec_struct*
_tmp7F2;const char*_tmp7F1;struct _tuple22*_tmp7F0;const char*_tmp7EF;const char*
_tmp7EE;struct Cyc_Arg_Set_spec_struct _tmp7ED;struct Cyc_Arg_Set_spec_struct*
_tmp7EC;const char*_tmp7EB;struct _tuple22*_tmp7EA;const char*_tmp7E9;const char*
_tmp7E8;struct Cyc_Arg_Set_spec_struct _tmp7E7;struct Cyc_Arg_Set_spec_struct*
_tmp7E6;const char*_tmp7E5;struct _tuple22*_tmp7E4;const char*_tmp7E3;const char*
_tmp7E2;struct Cyc_Arg_Set_spec_struct _tmp7E1;struct Cyc_Arg_Set_spec_struct*
_tmp7E0;const char*_tmp7DF;struct _tuple22*_tmp7DE;const char*_tmp7DD;const char*
_tmp7DC;struct Cyc_Arg_Set_spec_struct _tmp7DB;struct Cyc_Arg_Set_spec_struct*
_tmp7DA;const char*_tmp7D9;struct _tuple22*_tmp7D8;const char*_tmp7D7;const char*
_tmp7D6;struct Cyc_Arg_String_spec_struct _tmp7D5;struct Cyc_Arg_String_spec_struct*
_tmp7D4;const char*_tmp7D3;struct _tuple22*_tmp7D2;const char*_tmp7D1;const char*
_tmp7D0;struct Cyc_Arg_Set_spec_struct _tmp7CF;struct Cyc_Arg_Set_spec_struct*
_tmp7CE;const char*_tmp7CD;struct _tuple22*_tmp7CC;const char*_tmp7CB;const char*
_tmp7CA;struct Cyc_Arg_Set_spec_struct _tmp7C9;struct Cyc_Arg_Set_spec_struct*
_tmp7C8;const char*_tmp7C7;struct _tuple22*_tmp7C6;const char*_tmp7C5;const char*
_tmp7C4;struct Cyc_Arg_Set_spec_struct _tmp7C3;struct Cyc_Arg_Set_spec_struct*
_tmp7C2;const char*_tmp7C1;struct _tuple22*_tmp7C0;const char*_tmp7BF;const char*
_tmp7BE;struct Cyc_Arg_Clear_spec_struct _tmp7BD;struct Cyc_Arg_Clear_spec_struct*
_tmp7BC;const char*_tmp7BB;struct _tuple22*_tmp7BA;const char*_tmp7B9;const char*
_tmp7B8;struct Cyc_Arg_Unit_spec_struct _tmp7B7;struct Cyc_Arg_Unit_spec_struct*
_tmp7B6;const char*_tmp7B5;struct _tuple22*_tmp7B4;const char*_tmp7B3;const char*
_tmp7B2;struct Cyc_Arg_Set_spec_struct _tmp7B1;struct Cyc_Arg_Set_spec_struct*
_tmp7B0;const char*_tmp7AF;struct _tuple22*_tmp7AE[64];struct Cyc_List_List*options=(
_tmp7AE[63]=((_tmp7B4=_region_malloc(r,sizeof(*_tmp7B4)),((_tmp7B4->f1=((_tmp7B3="-noregions",
_tag_dyneither(_tmp7B3,sizeof(char),11))),((_tmp7B4->f2=0,((_tmp7B4->f3=((
_tmp7B2="",_tag_dyneither(_tmp7B2,sizeof(char),1))),((_tmp7B4->f4=(void*)((
_tmp7B0=_region_malloc(r,sizeof(*_tmp7B0)),((_tmp7B0[0]=((_tmp7B1.tag=3,((
_tmp7B1.f1=& Cyc_Absyn_no_regions,_tmp7B1)))),_tmp7B0)))),((_tmp7B4->f5=((_tmp7AF="Generate code that doesn't use regions",
_tag_dyneither(_tmp7AF,sizeof(char),39))),_tmp7B4)))))))))))),((_tmp7AE[62]=((
_tmp7BA=_region_malloc(r,sizeof(*_tmp7BA)),((_tmp7BA->f1=((_tmp7B9="-port",
_tag_dyneither(_tmp7B9,sizeof(char),6))),((_tmp7BA->f2=0,((_tmp7BA->f3=((_tmp7B8="",
_tag_dyneither(_tmp7B8,sizeof(char),1))),((_tmp7BA->f4=(void*)((_tmp7B6=
_region_malloc(r,sizeof(*_tmp7B6)),((_tmp7B6[0]=((_tmp7B7.tag=0,((_tmp7B7.f1=Cyc_set_port_c_code,
_tmp7B7)))),_tmp7B6)))),((_tmp7BA->f5=((_tmp7B5="Suggest how to port C code to Cyclone",
_tag_dyneither(_tmp7B5,sizeof(char),38))),_tmp7BA)))))))))))),((_tmp7AE[61]=((
_tmp7C0=_region_malloc(r,sizeof(*_tmp7C0)),((_tmp7C0->f1=((_tmp7BF="-detailedlocation",
_tag_dyneither(_tmp7BF,sizeof(char),18))),((_tmp7C0->f2=0,((_tmp7C0->f3=((
_tmp7BE="",_tag_dyneither(_tmp7BE,sizeof(char),1))),((_tmp7C0->f4=(void*)((
_tmp7BC=_region_malloc(r,sizeof(*_tmp7BC)),((_tmp7BC[0]=((_tmp7BD.tag=4,((
_tmp7BD.f1=& Cyc_Position_use_gcc_style_location,_tmp7BD)))),_tmp7BC)))),((
_tmp7C0->f5=((_tmp7BB="Try to give more detailed location information for errors",
_tag_dyneither(_tmp7BB,sizeof(char),58))),_tmp7C0)))))))))))),((_tmp7AE[60]=((
_tmp7C6=_region_malloc(r,sizeof(*_tmp7C6)),((_tmp7C6->f1=((_tmp7C5="-noregister",
_tag_dyneither(_tmp7C5,sizeof(char),12))),((_tmp7C6->f2=0,((_tmp7C6->f3=((
_tmp7C4="",_tag_dyneither(_tmp7C4,sizeof(char),1))),((_tmp7C6->f4=(void*)((
_tmp7C2=_region_malloc(r,sizeof(*_tmp7C2)),((_tmp7C2[0]=((_tmp7C3.tag=3,((
_tmp7C3.f1=& Cyc_Parse_no_register,_tmp7C3)))),_tmp7C2)))),((_tmp7C6->f5=((
_tmp7C1="Treat register storage class as public",_tag_dyneither(_tmp7C1,sizeof(
char),39))),_tmp7C6)))))))))))),((_tmp7AE[59]=((_tmp7CC=_region_malloc(r,sizeof(*
_tmp7CC)),((_tmp7CC->f1=((_tmp7CB="-warnnullchecks",_tag_dyneither(_tmp7CB,
sizeof(char),16))),((_tmp7CC->f2=0,((_tmp7CC->f3=((_tmp7CA="",_tag_dyneither(
_tmp7CA,sizeof(char),1))),((_tmp7CC->f4=(void*)((_tmp7C8=_region_malloc(r,
sizeof(*_tmp7C8)),((_tmp7C8[0]=((_tmp7C9.tag=3,((_tmp7C9.f1=& Cyc_Toc_warn_all_null_deref,
_tmp7C9)))),_tmp7C8)))),((_tmp7CC->f5=((_tmp7C7="Warn when any null check can't be eliminated",
_tag_dyneither(_tmp7C7,sizeof(char),45))),_tmp7CC)))))))))))),((_tmp7AE[58]=((
_tmp7D2=_region_malloc(r,sizeof(*_tmp7D2)),((_tmp7D2->f1=((_tmp7D1="-warnboundschecks",
_tag_dyneither(_tmp7D1,sizeof(char),18))),((_tmp7D2->f2=0,((_tmp7D2->f3=((
_tmp7D0="",_tag_dyneither(_tmp7D0,sizeof(char),1))),((_tmp7D2->f4=(void*)((
_tmp7CE=_region_malloc(r,sizeof(*_tmp7CE)),((_tmp7CE[0]=((_tmp7CF.tag=3,((
_tmp7CF.f1=& Cyc_Toc_warn_bounds_checks,_tmp7CF)))),_tmp7CE)))),((_tmp7D2->f5=((
_tmp7CD="Warn when bounds checks can't be eliminated",_tag_dyneither(_tmp7CD,
sizeof(char),44))),_tmp7D2)))))))))))),((_tmp7AE[57]=((_tmp7D8=_region_malloc(r,
sizeof(*_tmp7D8)),((_tmp7D8->f1=((_tmp7D7="-CI",_tag_dyneither(_tmp7D7,sizeof(
char),4))),((_tmp7D8->f2=0,((_tmp7D8->f3=((_tmp7D6=" <file>",_tag_dyneither(
_tmp7D6,sizeof(char),8))),((_tmp7D8->f4=(void*)((_tmp7D4=_region_malloc(r,
sizeof(*_tmp7D4)),((_tmp7D4[0]=((_tmp7D5.tag=5,((_tmp7D5.f1=Cyc_set_cyc_include,
_tmp7D5)))),_tmp7D4)))),((_tmp7D8->f5=((_tmp7D3="Set cyc_include.h to be <file>",
_tag_dyneither(_tmp7D3,sizeof(char),31))),_tmp7D8)))))))))))),((_tmp7AE[56]=((
_tmp7DE=_region_malloc(r,sizeof(*_tmp7DE)),((_tmp7DE->f1=((_tmp7DD="-nocyc_setjmp",
_tag_dyneither(_tmp7DD,sizeof(char),14))),((_tmp7DE->f2=0,((_tmp7DE->f3=((
_tmp7DC="",_tag_dyneither(_tmp7DC,sizeof(char),1))),((_tmp7DE->f4=(void*)((
_tmp7DA=_region_malloc(r,sizeof(*_tmp7DA)),((_tmp7DA[0]=((_tmp7DB.tag=3,((
_tmp7DB.f1=& Cyc_nocyc_setjmp_r,_tmp7DB)))),_tmp7DA)))),((_tmp7DE->f5=((_tmp7D9="Do not use compiler special file cyc_setjmp.h",
_tag_dyneither(_tmp7D9,sizeof(char),46))),_tmp7DE)))))))))))),((_tmp7AE[55]=((
_tmp7E4=_region_malloc(r,sizeof(*_tmp7E4)),((_tmp7E4->f1=((_tmp7E3="-printalleffects",
_tag_dyneither(_tmp7E3,sizeof(char),17))),((_tmp7E4->f2=0,((_tmp7E4->f3=((
_tmp7E2="",_tag_dyneither(_tmp7E2,sizeof(char),1))),((_tmp7E4->f4=(void*)((
_tmp7E0=_region_malloc(r,sizeof(*_tmp7E0)),((_tmp7E0[0]=((_tmp7E1.tag=3,((
_tmp7E1.f1=& Cyc_print_all_effects_r,_tmp7E1)))),_tmp7E0)))),((_tmp7E4->f5=((
_tmp7DF="Print effects for functions (type debugging)",_tag_dyneither(_tmp7DF,
sizeof(char),45))),_tmp7E4)))))))))))),((_tmp7AE[54]=((_tmp7EA=_region_malloc(r,
sizeof(*_tmp7EA)),((_tmp7EA->f1=((_tmp7E9="-printfullevars",_tag_dyneither(
_tmp7E9,sizeof(char),16))),((_tmp7EA->f2=0,((_tmp7EA->f3=((_tmp7E8="",
_tag_dyneither(_tmp7E8,sizeof(char),1))),((_tmp7EA->f4=(void*)((_tmp7E6=
_region_malloc(r,sizeof(*_tmp7E6)),((_tmp7E6[0]=((_tmp7E7.tag=3,((_tmp7E7.f1=&
Cyc_print_full_evars_r,_tmp7E7)))),_tmp7E6)))),((_tmp7EA->f5=((_tmp7E5="Print full information for evars (type debugging)",
_tag_dyneither(_tmp7E5,sizeof(char),50))),_tmp7EA)))))))))))),((_tmp7AE[53]=((
_tmp7F0=_region_malloc(r,sizeof(*_tmp7F0)),((_tmp7F0->f1=((_tmp7EF="-printallkinds",
_tag_dyneither(_tmp7EF,sizeof(char),15))),((_tmp7F0->f2=0,((_tmp7F0->f3=((
_tmp7EE="",_tag_dyneither(_tmp7EE,sizeof(char),1))),((_tmp7F0->f4=(void*)((
_tmp7EC=_region_malloc(r,sizeof(*_tmp7EC)),((_tmp7EC[0]=((_tmp7ED.tag=3,((
_tmp7ED.f1=& Cyc_print_all_kinds_r,_tmp7ED)))),_tmp7EC)))),((_tmp7F0->f5=((
_tmp7EB="Always print kinds of type variables",_tag_dyneither(_tmp7EB,sizeof(
char),37))),_tmp7F0)))))))))))),((_tmp7AE[52]=((_tmp7F6=_region_malloc(r,sizeof(*
_tmp7F6)),((_tmp7F6->f1=((_tmp7F5="-printalltvars",_tag_dyneither(_tmp7F5,
sizeof(char),15))),((_tmp7F6->f2=0,((_tmp7F6->f3=((_tmp7F4="",_tag_dyneither(
_tmp7F4,sizeof(char),1))),((_tmp7F6->f4=(void*)((_tmp7F2=_region_malloc(r,
sizeof(*_tmp7F2)),((_tmp7F2[0]=((_tmp7F3.tag=3,((_tmp7F3.f1=& Cyc_print_all_tvars_r,
_tmp7F3)))),_tmp7F2)))),((_tmp7F6->f5=((_tmp7F1="Print all type variables (even implicit default effects)",
_tag_dyneither(_tmp7F1,sizeof(char),57))),_tmp7F6)))))))))))),((_tmp7AE[51]=((
_tmp7FC=_region_malloc(r,sizeof(*_tmp7FC)),((_tmp7FC->f1=((_tmp7FB="-noexpandtypedefs",
_tag_dyneither(_tmp7FB,sizeof(char),18))),((_tmp7FC->f2=0,((_tmp7FC->f3=((
_tmp7FA="",_tag_dyneither(_tmp7FA,sizeof(char),1))),((_tmp7FC->f4=(void*)((
_tmp7F8=_region_malloc(r,sizeof(*_tmp7F8)),((_tmp7F8[0]=((_tmp7F9.tag=3,((
_tmp7F9.f1=& Cyc_noexpand_r,_tmp7F9)))),_tmp7F8)))),((_tmp7FC->f5=((_tmp7F7="Don't expand typedefs in pretty printing",
_tag_dyneither(_tmp7F7,sizeof(char),41))),_tmp7FC)))))))))))),((_tmp7AE[50]=((
_tmp802=_region_malloc(r,sizeof(*_tmp802)),((_tmp802->f1=((_tmp801="-noremoveunused",
_tag_dyneither(_tmp801,sizeof(char),16))),((_tmp802->f2=0,((_tmp802->f3=((
_tmp800="",_tag_dyneither(_tmp800,sizeof(char),1))),((_tmp802->f4=(void*)((
_tmp7FE=_region_malloc(r,sizeof(*_tmp7FE)),((_tmp7FE[0]=((_tmp7FF.tag=3,((
_tmp7FF.f1=& Cyc_noshake_r,_tmp7FF)))),_tmp7FE)))),((_tmp802->f5=((_tmp7FD="Don't remove externed variables that aren't used",
_tag_dyneither(_tmp7FD,sizeof(char),49))),_tmp802)))))))))))),((_tmp7AE[49]=((
_tmp808=_region_malloc(r,sizeof(*_tmp808)),((_tmp808->f1=((_tmp807="-nogc",
_tag_dyneither(_tmp807,sizeof(char),6))),((_tmp808->f2=0,((_tmp808->f3=((_tmp806="",
_tag_dyneither(_tmp806,sizeof(char),1))),((_tmp808->f4=(void*)((_tmp804=
_region_malloc(r,sizeof(*_tmp804)),((_tmp804[0]=((_tmp805.tag=3,((_tmp805.f1=&
Cyc_nogc_r,_tmp805)))),_tmp804)))),((_tmp808->f5=((_tmp803="Don't link in the garbage collector",
_tag_dyneither(_tmp803,sizeof(char),36))),_tmp808)))))))))))),((_tmp7AE[48]=((
_tmp80E=_region_malloc(r,sizeof(*_tmp80E)),((_tmp80E->f1=((_tmp80D="-nocyc",
_tag_dyneither(_tmp80D,sizeof(char),7))),((_tmp80E->f2=0,((_tmp80E->f3=((_tmp80C="",
_tag_dyneither(_tmp80C,sizeof(char),1))),((_tmp80E->f4=(void*)((_tmp80A=
_region_malloc(r,sizeof(*_tmp80A)),((_tmp80A[0]=((_tmp80B.tag=0,((_tmp80B.f1=Cyc_set_nocyc,
_tmp80B)))),_tmp80A)))),((_tmp80E->f5=((_tmp809="Don't add implicit namespace Cyc",
_tag_dyneither(_tmp809,sizeof(char),33))),_tmp80E)))))))))))),((_tmp7AE[47]=((
_tmp814=_region_malloc(r,sizeof(*_tmp814)),((_tmp814->f1=((_tmp813="-no-cpp-precomp",
_tag_dyneither(_tmp813,sizeof(char),16))),((_tmp814->f2=0,((_tmp814->f3=((
_tmp812="",_tag_dyneither(_tmp812,sizeof(char),1))),((_tmp814->f4=(void*)((
_tmp810=_region_malloc(r,sizeof(*_tmp810)),((_tmp810[0]=((_tmp811.tag=0,((
_tmp811.f1=Cyc_set_nocppprecomp,_tmp811)))),_tmp810)))),((_tmp814->f5=((_tmp80F="Don't do smart preprocessing (mac only)",
_tag_dyneither(_tmp80F,sizeof(char),40))),_tmp814)))))))))))),((_tmp7AE[46]=((
_tmp81A=_region_malloc(r,sizeof(*_tmp81A)),((_tmp81A->f1=((_tmp819="-use-cpp",
_tag_dyneither(_tmp819,sizeof(char),9))),((_tmp81A->f2=0,((_tmp81A->f3=((_tmp818="<path>",
_tag_dyneither(_tmp818,sizeof(char),7))),((_tmp81A->f4=(void*)((_tmp816=
_region_malloc(r,sizeof(*_tmp816)),((_tmp816[0]=((_tmp817.tag=5,((_tmp817.f1=Cyc_set_cpp,
_tmp817)))),_tmp816)))),((_tmp81A->f5=((_tmp815="Indicate which preprocessor to use",
_tag_dyneither(_tmp815,sizeof(char),35))),_tmp81A)))))))))))),((_tmp7AE[45]=((
_tmp820=_region_malloc(r,sizeof(*_tmp820)),((_tmp820->f1=((_tmp81F="--inline-checks",
_tag_dyneither(_tmp81F,sizeof(char),16))),((_tmp820->f2=0,((_tmp820->f3=((
_tmp81E="",_tag_dyneither(_tmp81E,sizeof(char),1))),((_tmp820->f4=(void*)((
_tmp81C=_region_malloc(r,sizeof(*_tmp81C)),((_tmp81C[0]=((_tmp81D.tag=0,((
_tmp81D.f1=Cyc_set_inline_functions,_tmp81D)))),_tmp81C)))),((_tmp820->f5=((
_tmp81B="Inline bounds checks instead of #define",_tag_dyneither(_tmp81B,sizeof(
char),40))),_tmp820)))))))))))),((_tmp7AE[44]=((_tmp826=_region_malloc(r,sizeof(*
_tmp826)),((_tmp826->f1=((_tmp825="--noboundschecks",_tag_dyneither(_tmp825,
sizeof(char),17))),((_tmp826->f2=0,((_tmp826->f3=((_tmp824="",_tag_dyneither(
_tmp824,sizeof(char),1))),((_tmp826->f4=(void*)((_tmp822=_region_malloc(r,
sizeof(*_tmp822)),((_tmp822[0]=((_tmp823.tag=0,((_tmp823.f1=Cyc_set_noboundschecks,
_tmp823)))),_tmp822)))),((_tmp826->f5=((_tmp821="Disable bounds checks",
_tag_dyneither(_tmp821,sizeof(char),22))),_tmp826)))))))))))),((_tmp7AE[43]=((
_tmp82C=_region_malloc(r,sizeof(*_tmp82C)),((_tmp82C->f1=((_tmp82B="--nonullchecks",
_tag_dyneither(_tmp82B,sizeof(char),15))),((_tmp82C->f2=0,((_tmp82C->f3=((
_tmp82A="",_tag_dyneither(_tmp82A,sizeof(char),1))),((_tmp82C->f4=(void*)((
_tmp828=_region_malloc(r,sizeof(*_tmp828)),((_tmp828[0]=((_tmp829.tag=0,((
_tmp829.f1=Cyc_set_nonullchecks,_tmp829)))),_tmp828)))),((_tmp82C->f5=((_tmp827="Disable null checks",
_tag_dyneither(_tmp827,sizeof(char),20))),_tmp82C)))))))))))),((_tmp7AE[42]=((
_tmp832=_region_malloc(r,sizeof(*_tmp832)),((_tmp832->f1=((_tmp831="--nochecks",
_tag_dyneither(_tmp831,sizeof(char),11))),((_tmp832->f2=0,((_tmp832->f3=((
_tmp830="",_tag_dyneither(_tmp830,sizeof(char),1))),((_tmp832->f4=(void*)((
_tmp82E=_region_malloc(r,sizeof(*_tmp82E)),((_tmp82E[0]=((_tmp82F.tag=0,((
_tmp82F.f1=Cyc_set_nochecks,_tmp82F)))),_tmp82E)))),((_tmp832->f5=((_tmp82D="Disable bounds/null checks",
_tag_dyneither(_tmp82D,sizeof(char),27))),_tmp832)))))))))))),((_tmp7AE[41]=((
_tmp838=_region_malloc(r,sizeof(*_tmp838)),((_tmp838->f1=((_tmp837="--lineno",
_tag_dyneither(_tmp837,sizeof(char),9))),((_tmp838->f2=0,((_tmp838->f3=((_tmp836="",
_tag_dyneither(_tmp836,sizeof(char),1))),((_tmp838->f4=(void*)((_tmp834=
_region_malloc(r,sizeof(*_tmp834)),((_tmp834[0]=((_tmp835.tag=0,((_tmp835.f1=Cyc_set_lineno,
_tmp835)))),_tmp834)))),((_tmp838->f5=((_tmp833="Generate line numbers for debugging",
_tag_dyneither(_tmp833,sizeof(char),36))),_tmp838)))))))))))),((_tmp7AE[40]=((
_tmp83E=_region_malloc(r,sizeof(*_tmp83E)),((_tmp83E->f1=((_tmp83D="-save-c",
_tag_dyneither(_tmp83D,sizeof(char),8))),((_tmp83E->f2=0,((_tmp83E->f3=((_tmp83C="",
_tag_dyneither(_tmp83C,sizeof(char),1))),((_tmp83E->f4=(void*)((_tmp83A=
_region_malloc(r,sizeof(*_tmp83A)),((_tmp83A[0]=((_tmp83B.tag=3,((_tmp83B.f1=&
Cyc_save_c_r,_tmp83B)))),_tmp83A)))),((_tmp83E->f5=((_tmp839="Don't delete temporary C files",
_tag_dyneither(_tmp839,sizeof(char),31))),_tmp83E)))))))))))),((_tmp7AE[39]=((
_tmp844=_region_malloc(r,sizeof(*_tmp844)),((_tmp844->f1=((_tmp843="-save-temps",
_tag_dyneither(_tmp843,sizeof(char),12))),((_tmp844->f2=0,((_tmp844->f3=((
_tmp842="",_tag_dyneither(_tmp842,sizeof(char),1))),((_tmp844->f4=(void*)((
_tmp840=_region_malloc(r,sizeof(*_tmp840)),((_tmp840[0]=((_tmp841.tag=0,((
_tmp841.f1=Cyc_set_save_temps,_tmp841)))),_tmp840)))),((_tmp844->f5=((_tmp83F="Don't delete temporary files",
_tag_dyneither(_tmp83F,sizeof(char),29))),_tmp844)))))))))))),((_tmp7AE[38]=((
_tmp84A=_region_malloc(r,sizeof(*_tmp84A)),((_tmp84A->f1=((_tmp849="-c-comp",
_tag_dyneither(_tmp849,sizeof(char),8))),((_tmp84A->f2=0,((_tmp84A->f3=((_tmp848=" <compiler>",
_tag_dyneither(_tmp848,sizeof(char),12))),((_tmp84A->f4=(void*)((_tmp846=
_region_malloc(r,sizeof(*_tmp846)),((_tmp846[0]=((_tmp847.tag=5,((_tmp847.f1=Cyc_set_c_compiler,
_tmp847)))),_tmp846)))),((_tmp84A->f5=((_tmp845="Produce C output for the given compiler",
_tag_dyneither(_tmp845,sizeof(char),40))),_tmp84A)))))))))))),((_tmp7AE[37]=((
_tmp850=_region_malloc(r,sizeof(*_tmp850)),((_tmp850->f1=((_tmp84F="-un-gcc",
_tag_dyneither(_tmp84F,sizeof(char),8))),((_tmp850->f2=0,((_tmp850->f3=((_tmp84E="",
_tag_dyneither(_tmp84E,sizeof(char),1))),((_tmp850->f4=(void*)((_tmp84C=
_region_malloc(r,sizeof(*_tmp84C)),((_tmp84C[0]=((_tmp84D.tag=0,((_tmp84D.f1=Cyc_set_tovc,
_tmp84D)))),_tmp84C)))),((_tmp850->f5=((_tmp84B="Avoid gcc extensions in C output (except for attributes)",
_tag_dyneither(_tmp84B,sizeof(char),57))),_tmp850)))))))))))),((_tmp7AE[36]=((
_tmp856=_region_malloc(r,sizeof(*_tmp856)),((_tmp856->f1=((_tmp855="-elim-statement-expressions",
_tag_dyneither(_tmp855,sizeof(char),28))),((_tmp856->f2=0,((_tmp856->f3=((
_tmp854="",_tag_dyneither(_tmp854,sizeof(char),1))),((_tmp856->f4=(void*)((
_tmp852=_region_malloc(r,sizeof(*_tmp852)),((_tmp852[0]=((_tmp853.tag=3,((
_tmp853.f1=& Cyc_elim_se_r,_tmp853)))),_tmp852)))),((_tmp856->f5=((_tmp851="Avoid statement expressions in C output (implies --inline-checks)",
_tag_dyneither(_tmp851,sizeof(char),66))),_tmp856)))))))))))),((_tmp7AE[35]=((
_tmp85C=_region_malloc(r,sizeof(*_tmp85C)),((_tmp85C->f1=((_tmp85B="-up",
_tag_dyneither(_tmp85B,sizeof(char),4))),((_tmp85C->f2=0,((_tmp85C->f3=((_tmp85A="",
_tag_dyneither(_tmp85A,sizeof(char),1))),((_tmp85C->f4=(void*)((_tmp858=
_region_malloc(r,sizeof(*_tmp858)),((_tmp858[0]=((_tmp859.tag=4,((_tmp859.f1=&
Cyc_pp_r,_tmp859)))),_tmp858)))),((_tmp85C->f5=((_tmp857="Ugly print the C code that Cyclone generates (default)",
_tag_dyneither(_tmp857,sizeof(char),55))),_tmp85C)))))))))))),((_tmp7AE[34]=((
_tmp862=_region_malloc(r,sizeof(*_tmp862)),((_tmp862->f1=((_tmp861="-pp",
_tag_dyneither(_tmp861,sizeof(char),4))),((_tmp862->f2=0,((_tmp862->f3=((_tmp860="",
_tag_dyneither(_tmp860,sizeof(char),1))),((_tmp862->f4=(void*)((_tmp85E=
_region_malloc(r,sizeof(*_tmp85E)),((_tmp85E[0]=((_tmp85F.tag=3,((_tmp85F.f1=&
Cyc_pp_r,_tmp85F)))),_tmp85E)))),((_tmp862->f5=((_tmp85D="Pretty print the C code that Cyclone generates",
_tag_dyneither(_tmp85D,sizeof(char),47))),_tmp862)))))))))))),((_tmp7AE[33]=((
_tmp868=_region_malloc(r,sizeof(*_tmp868)),((_tmp868->f1=((_tmp867="-ic",
_tag_dyneither(_tmp867,sizeof(char),4))),((_tmp868->f2=0,((_tmp868->f3=((_tmp866="",
_tag_dyneither(_tmp866,sizeof(char),1))),((_tmp868->f4=(void*)((_tmp864=
_region_malloc(r,sizeof(*_tmp864)),((_tmp864[0]=((_tmp865.tag=3,((_tmp865.f1=&
Cyc_ic_r,_tmp865)))),_tmp864)))),((_tmp868->f5=((_tmp863="Activate the link-checker",
_tag_dyneither(_tmp863,sizeof(char),26))),_tmp868)))))))))))),((_tmp7AE[32]=((
_tmp86E=_region_malloc(r,sizeof(*_tmp86E)),((_tmp86E->f1=((_tmp86D="-stopafter-toc",
_tag_dyneither(_tmp86D,sizeof(char),15))),((_tmp86E->f2=0,((_tmp86E->f3=((
_tmp86C="",_tag_dyneither(_tmp86C,sizeof(char),1))),((_tmp86E->f4=(void*)((
_tmp86A=_region_malloc(r,sizeof(*_tmp86A)),((_tmp86A[0]=((_tmp86B.tag=3,((
_tmp86B.f1=& Cyc_toc_r,_tmp86B)))),_tmp86A)))),((_tmp86E->f5=((_tmp869="Stop after translation to C",
_tag_dyneither(_tmp869,sizeof(char),28))),_tmp86E)))))))))))),((_tmp7AE[31]=((
_tmp874=_region_malloc(r,sizeof(*_tmp874)),((_tmp874->f1=((_tmp873="-stopafter-cf",
_tag_dyneither(_tmp873,sizeof(char),14))),((_tmp874->f2=0,((_tmp874->f3=((
_tmp872="",_tag_dyneither(_tmp872,sizeof(char),1))),((_tmp874->f4=(void*)((
_tmp870=_region_malloc(r,sizeof(*_tmp870)),((_tmp870[0]=((_tmp871.tag=3,((
_tmp871.f1=& Cyc_cf_r,_tmp871)))),_tmp870)))),((_tmp874->f5=((_tmp86F="Stop after control-flow checking",
_tag_dyneither(_tmp86F,sizeof(char),33))),_tmp874)))))))))))),((_tmp7AE[30]=((
_tmp87A=_region_malloc(r,sizeof(*_tmp87A)),((_tmp87A->f1=((_tmp879="-noprint",
_tag_dyneither(_tmp879,sizeof(char),9))),((_tmp87A->f2=0,((_tmp87A->f3=((_tmp878="",
_tag_dyneither(_tmp878,sizeof(char),1))),((_tmp87A->f4=(void*)((_tmp876=
_region_malloc(r,sizeof(*_tmp876)),((_tmp876[0]=((_tmp877.tag=3,((_tmp877.f1=&
Cyc_noprint_r,_tmp877)))),_tmp876)))),((_tmp87A->f5=((_tmp875="Do not print output (when stopping early)",
_tag_dyneither(_tmp875,sizeof(char),42))),_tmp87A)))))))))))),((_tmp7AE[29]=((
_tmp880=_region_malloc(r,sizeof(*_tmp880)),((_tmp880->f1=((_tmp87F="-stopafter-tc",
_tag_dyneither(_tmp87F,sizeof(char),14))),((_tmp880->f2=0,((_tmp880->f3=((
_tmp87E="",_tag_dyneither(_tmp87E,sizeof(char),1))),((_tmp880->f4=(void*)((
_tmp87C=_region_malloc(r,sizeof(*_tmp87C)),((_tmp87C[0]=((_tmp87D.tag=3,((
_tmp87D.f1=& Cyc_tc_r,_tmp87D)))),_tmp87C)))),((_tmp880->f5=((_tmp87B="Stop after type checking",
_tag_dyneither(_tmp87B,sizeof(char),25))),_tmp880)))))))))))),((_tmp7AE[28]=((
_tmp886=_region_malloc(r,sizeof(*_tmp886)),((_tmp886->f1=((_tmp885="-stopafter-parse",
_tag_dyneither(_tmp885,sizeof(char),17))),((_tmp886->f2=0,((_tmp886->f3=((
_tmp884="",_tag_dyneither(_tmp884,sizeof(char),1))),((_tmp886->f4=(void*)((
_tmp882=_region_malloc(r,sizeof(*_tmp882)),((_tmp882[0]=((_tmp883.tag=3,((
_tmp883.f1=& Cyc_parseonly_r,_tmp883)))),_tmp882)))),((_tmp886->f5=((_tmp881="Stop after parsing",
_tag_dyneither(_tmp881,sizeof(char),19))),_tmp886)))))))))))),((_tmp7AE[27]=((
_tmp88C=_region_malloc(r,sizeof(*_tmp88C)),((_tmp88C->f1=((_tmp88B="-E",
_tag_dyneither(_tmp88B,sizeof(char),3))),((_tmp88C->f2=0,((_tmp88C->f3=((_tmp88A="",
_tag_dyneither(_tmp88A,sizeof(char),1))),((_tmp88C->f4=(void*)((_tmp888=
_region_malloc(r,sizeof(*_tmp888)),((_tmp888[0]=((_tmp889.tag=3,((_tmp889.f1=&
Cyc_stop_after_cpp_r,_tmp889)))),_tmp888)))),((_tmp88C->f5=((_tmp887="Stop after preprocessing",
_tag_dyneither(_tmp887,sizeof(char),25))),_tmp88C)))))))))))),((_tmp7AE[26]=((
_tmp892=_region_malloc(r,sizeof(*_tmp892)),((_tmp892->f1=((_tmp891="-Wall",
_tag_dyneither(_tmp891,sizeof(char),6))),((_tmp892->f2=0,((_tmp892->f3=((_tmp890="",
_tag_dyneither(_tmp890,sizeof(char),1))),((_tmp892->f4=(void*)((_tmp88E=
_region_malloc(r,sizeof(*_tmp88E)),((_tmp88E[0]=((_tmp88F.tag=0,((_tmp88F.f1=Cyc_set_all_warnings,
_tmp88F)))),_tmp88E)))),((_tmp892->f5=((_tmp88D="Turn on all warnings",
_tag_dyneither(_tmp88D,sizeof(char),21))),_tmp892)))))))))))),((_tmp7AE[25]=((
_tmp898=_region_malloc(r,sizeof(*_tmp898)),((_tmp898->f1=((_tmp897="-b",
_tag_dyneither(_tmp897,sizeof(char),3))),((_tmp898->f2=0,((_tmp898->f3=((_tmp896="<arch>",
_tag_dyneither(_tmp896,sizeof(char),7))),((_tmp898->f4=(void*)((_tmp894=
_region_malloc(r,sizeof(*_tmp894)),((_tmp894[0]=((_tmp895.tag=5,((_tmp895.f1=Cyc_set_target_arch,
_tmp895)))),_tmp894)))),((_tmp898->f5=((_tmp893="Set target architecture; implies -stopafter-toc",
_tag_dyneither(_tmp893,sizeof(char),48))),_tmp898)))))))))))),((_tmp7AE[24]=((
_tmp89E=_region_malloc(r,sizeof(*_tmp89E)),((_tmp89E->f1=((_tmp89D="-MT",
_tag_dyneither(_tmp89D,sizeof(char),4))),((_tmp89E->f2=0,((_tmp89E->f3=((_tmp89C=" <target>",
_tag_dyneither(_tmp89C,sizeof(char),10))),((_tmp89E->f4=(void*)((_tmp89A=
_region_malloc(r,sizeof(*_tmp89A)),((_tmp89A[0]=((_tmp89B.tag=5,((_tmp89B.f1=Cyc_set_dependencies_target,
_tmp89B)))),_tmp89A)))),((_tmp89E->f5=((_tmp899="Give target for dependencies",
_tag_dyneither(_tmp899,sizeof(char),29))),_tmp89E)))))))))))),((_tmp7AE[23]=((
_tmp8A4=_region_malloc(r,sizeof(*_tmp8A4)),((_tmp8A4->f1=((_tmp8A3="-MG",
_tag_dyneither(_tmp8A3,sizeof(char),4))),((_tmp8A4->f2=0,((_tmp8A4->f3=((_tmp8A2="",
_tag_dyneither(_tmp8A2,sizeof(char),1))),((_tmp8A4->f4=(void*)((_tmp8A0=
_region_malloc(r,sizeof(*_tmp8A0)),((_tmp8A0[0]=((_tmp8A1.tag=1,((_tmp8A1.f1=Cyc_add_cpparg,
_tmp8A1)))),_tmp8A0)))),((_tmp8A4->f5=((_tmp89F="When producing dependencies assume that missing files are generated",
_tag_dyneither(_tmp89F,sizeof(char),68))),_tmp8A4)))))))))))),((_tmp7AE[22]=((
_tmp8AA=_region_malloc(r,sizeof(*_tmp8AA)),((_tmp8AA->f1=((_tmp8A9="-M",
_tag_dyneither(_tmp8A9,sizeof(char),3))),((_tmp8AA->f2=0,((_tmp8AA->f3=((_tmp8A8="",
_tag_dyneither(_tmp8A8,sizeof(char),1))),((_tmp8AA->f4=(void*)((_tmp8A6=
_region_malloc(r,sizeof(*_tmp8A6)),((_tmp8A6[0]=((_tmp8A7.tag=0,((_tmp8A7.f1=Cyc_set_produce_dependencies,
_tmp8A7)))),_tmp8A6)))),((_tmp8AA->f5=((_tmp8A5="Produce dependencies",
_tag_dyneither(_tmp8A5,sizeof(char),21))),_tmp8AA)))))))))))),((_tmp7AE[21]=((
_tmp8B0=_region_malloc(r,sizeof(*_tmp8B0)),((_tmp8B0->f1=((_tmp8AF="-S",
_tag_dyneither(_tmp8AF,sizeof(char),3))),((_tmp8B0->f2=0,((_tmp8B0->f3=((_tmp8AE="",
_tag_dyneither(_tmp8AE,sizeof(char),1))),((_tmp8B0->f4=(void*)((_tmp8AC=
_region_malloc(r,sizeof(*_tmp8AC)),((_tmp8AC[0]=((_tmp8AD.tag=0,((_tmp8AD.f1=Cyc_set_stop_after_asmfile,
_tmp8AD)))),_tmp8AC)))),((_tmp8B0->f5=((_tmp8AB="Stop after producing assembly code",
_tag_dyneither(_tmp8AB,sizeof(char),35))),_tmp8B0)))))))))))),((_tmp7AE[20]=((
_tmp8B6=_region_malloc(r,sizeof(*_tmp8B6)),((_tmp8B6->f1=((_tmp8B5="-pa",
_tag_dyneither(_tmp8B5,sizeof(char),4))),((_tmp8B6->f2=0,((_tmp8B6->f3=((_tmp8B4="",
_tag_dyneither(_tmp8B4,sizeof(char),1))),((_tmp8B6->f4=(void*)((_tmp8B2=
_region_malloc(r,sizeof(*_tmp8B2)),((_tmp8B2[0]=((_tmp8B3.tag=0,((_tmp8B3.f1=Cyc_set_pa,
_tmp8B3)))),_tmp8B2)))),((_tmp8B6->f5=((_tmp8B1="Compile for profiling with aprof",
_tag_dyneither(_tmp8B1,sizeof(char),33))),_tmp8B6)))))))))))),((_tmp7AE[19]=((
_tmp8BC=_region_malloc(r,sizeof(*_tmp8BC)),((_tmp8BC->f1=((_tmp8BB="-pg",
_tag_dyneither(_tmp8BB,sizeof(char),4))),((_tmp8BC->f2=0,((_tmp8BC->f3=((_tmp8BA="",
_tag_dyneither(_tmp8BA,sizeof(char),1))),((_tmp8BC->f4=(void*)((_tmp8B8=
_region_malloc(r,sizeof(*_tmp8B8)),((_tmp8B8[0]=((_tmp8B9.tag=0,((_tmp8B9.f1=Cyc_set_pg,
_tmp8B9)))),_tmp8B8)))),((_tmp8BC->f5=((_tmp8B7="Compile for profiling with gprof",
_tag_dyneither(_tmp8B7,sizeof(char),33))),_tmp8BC)))))))))))),((_tmp7AE[18]=((
_tmp8C2=_region_malloc(r,sizeof(*_tmp8C2)),((_tmp8C2->f1=((_tmp8C1="-p",
_tag_dyneither(_tmp8C1,sizeof(char),3))),((_tmp8C2->f2=0,((_tmp8C2->f3=((_tmp8C0="",
_tag_dyneither(_tmp8C0,sizeof(char),1))),((_tmp8C2->f4=(void*)((_tmp8BE=
_region_malloc(r,sizeof(*_tmp8BE)),((_tmp8BE[0]=((_tmp8BF.tag=1,((_tmp8BF.f1=Cyc_add_ccarg,
_tmp8BF)))),_tmp8BE)))),((_tmp8C2->f5=((_tmp8BD="Compile for profiling with prof",
_tag_dyneither(_tmp8BD,sizeof(char),32))),_tmp8C2)))))))))))),((_tmp7AE[17]=((
_tmp8C8=_region_malloc(r,sizeof(*_tmp8C8)),((_tmp8C8->f1=((_tmp8C7="-g",
_tag_dyneither(_tmp8C7,sizeof(char),3))),((_tmp8C8->f2=0,((_tmp8C8->f3=((_tmp8C6="",
_tag_dyneither(_tmp8C6,sizeof(char),1))),((_tmp8C8->f4=(void*)((_tmp8C4=
_region_malloc(r,sizeof(*_tmp8C4)),((_tmp8C4[0]=((_tmp8C5.tag=1,((_tmp8C5.f1=Cyc_add_ccarg,
_tmp8C5)))),_tmp8C4)))),((_tmp8C8->f5=((_tmp8C3="Compile for debugging",
_tag_dyneither(_tmp8C3,sizeof(char),22))),_tmp8C8)))))))))))),((_tmp7AE[16]=((
_tmp8CE=_region_malloc(r,sizeof(*_tmp8CE)),((_tmp8CE->f1=((_tmp8CD="-fomit-frame-pointer",
_tag_dyneither(_tmp8CD,sizeof(char),21))),((_tmp8CE->f2=0,((_tmp8CE->f3=((
_tmp8CC="",_tag_dyneither(_tmp8CC,sizeof(char),1))),((_tmp8CE->f4=(void*)((
_tmp8CA=_region_malloc(r,sizeof(*_tmp8CA)),((_tmp8CA[0]=((_tmp8CB.tag=1,((
_tmp8CB.f1=Cyc_add_ccarg,_tmp8CB)))),_tmp8CA)))),((_tmp8CE->f5=((_tmp8C9="Omit frame pointer",
_tag_dyneither(_tmp8C9,sizeof(char),19))),_tmp8CE)))))))))))),((_tmp7AE[15]=((
_tmp8D4=_region_malloc(r,sizeof(*_tmp8D4)),((_tmp8D4->f1=((_tmp8D3="-O3",
_tag_dyneither(_tmp8D3,sizeof(char),4))),((_tmp8D4->f2=0,((_tmp8D4->f3=((_tmp8D2="",
_tag_dyneither(_tmp8D2,sizeof(char),1))),((_tmp8D4->f4=(void*)((_tmp8D0=
_region_malloc(r,sizeof(*_tmp8D0)),((_tmp8D0[0]=((_tmp8D1.tag=1,((_tmp8D1.f1=Cyc_add_ccarg,
_tmp8D1)))),_tmp8D0)))),((_tmp8D4->f5=((_tmp8CF="Even more optimization",
_tag_dyneither(_tmp8CF,sizeof(char),23))),_tmp8D4)))))))))))),((_tmp7AE[14]=((
_tmp8DA=_region_malloc(r,sizeof(*_tmp8DA)),((_tmp8DA->f1=((_tmp8D9="-O2",
_tag_dyneither(_tmp8D9,sizeof(char),4))),((_tmp8DA->f2=0,((_tmp8DA->f3=((_tmp8D8="",
_tag_dyneither(_tmp8D8,sizeof(char),1))),((_tmp8DA->f4=(void*)((_tmp8D6=
_region_malloc(r,sizeof(*_tmp8D6)),((_tmp8D6[0]=((_tmp8D7.tag=1,((_tmp8D7.f1=Cyc_add_ccarg,
_tmp8D7)))),_tmp8D6)))),((_tmp8DA->f5=((_tmp8D5="A higher level of optimization",
_tag_dyneither(_tmp8D5,sizeof(char),31))),_tmp8DA)))))))))))),((_tmp7AE[13]=((
_tmp8E0=_region_malloc(r,sizeof(*_tmp8E0)),((_tmp8E0->f1=((_tmp8DF="-O",
_tag_dyneither(_tmp8DF,sizeof(char),3))),((_tmp8E0->f2=0,((_tmp8E0->f3=((_tmp8DE="",
_tag_dyneither(_tmp8DE,sizeof(char),1))),((_tmp8E0->f4=(void*)((_tmp8DC=
_region_malloc(r,sizeof(*_tmp8DC)),((_tmp8DC[0]=((_tmp8DD.tag=1,((_tmp8DD.f1=Cyc_add_ccarg,
_tmp8DD)))),_tmp8DC)))),((_tmp8E0->f5=((_tmp8DB="Optimize",_tag_dyneither(
_tmp8DB,sizeof(char),9))),_tmp8E0)))))))))))),((_tmp7AE[12]=((_tmp8E6=
_region_malloc(r,sizeof(*_tmp8E6)),((_tmp8E6->f1=((_tmp8E5="-O0",_tag_dyneither(
_tmp8E5,sizeof(char),4))),((_tmp8E6->f2=0,((_tmp8E6->f3=((_tmp8E4="",
_tag_dyneither(_tmp8E4,sizeof(char),1))),((_tmp8E6->f4=(void*)((_tmp8E2=
_region_malloc(r,sizeof(*_tmp8E2)),((_tmp8E2[0]=((_tmp8E3.tag=1,((_tmp8E3.f1=Cyc_add_ccarg,
_tmp8E3)))),_tmp8E2)))),((_tmp8E6->f5=((_tmp8E1="Don't optimize",_tag_dyneither(
_tmp8E1,sizeof(char),15))),_tmp8E6)))))))))))),((_tmp7AE[11]=((_tmp8EC=
_region_malloc(r,sizeof(*_tmp8EC)),((_tmp8EC->f1=((_tmp8EB="-s",_tag_dyneither(
_tmp8EB,sizeof(char),3))),((_tmp8EC->f2=0,((_tmp8EC->f3=((_tmp8EA="",
_tag_dyneither(_tmp8EA,sizeof(char),1))),((_tmp8EC->f4=(void*)((_tmp8E8=
_region_malloc(r,sizeof(*_tmp8E8)),((_tmp8E8[0]=((_tmp8E9.tag=1,((_tmp8E9.f1=Cyc_add_ccarg,
_tmp8E9)))),_tmp8E8)))),((_tmp8EC->f5=((_tmp8E7="Remove all symbol table and relocation info from executable",
_tag_dyneither(_tmp8E7,sizeof(char),60))),_tmp8EC)))))))))))),((_tmp7AE[10]=((
_tmp8F2=_region_malloc(r,sizeof(*_tmp8F2)),((_tmp8F2->f1=((_tmp8F1="-x",
_tag_dyneither(_tmp8F1,sizeof(char),3))),((_tmp8F2->f2=0,((_tmp8F2->f3=((_tmp8F0=" <language>",
_tag_dyneither(_tmp8F0,sizeof(char),12))),((_tmp8F2->f4=(void*)((_tmp8EE=
_region_malloc(r,sizeof(*_tmp8EE)),((_tmp8EE[0]=((_tmp8EF.tag=5,((_tmp8EF.f1=Cyc_set_inputtype,
_tmp8EF)))),_tmp8EE)))),((_tmp8F2->f5=((_tmp8ED="Specify <language> for the following input files",
_tag_dyneither(_tmp8ED,sizeof(char),49))),_tmp8F2)))))))))))),((_tmp7AE[9]=((
_tmp8F8=_region_malloc(r,sizeof(*_tmp8F8)),((_tmp8F8->f1=((_tmp8F7="-c",
_tag_dyneither(_tmp8F7,sizeof(char),3))),((_tmp8F8->f2=0,((_tmp8F8->f3=((_tmp8F6="",
_tag_dyneither(_tmp8F6,sizeof(char),1))),((_tmp8F8->f4=(void*)((_tmp8F4=
_region_malloc(r,sizeof(*_tmp8F4)),((_tmp8F4[0]=((_tmp8F5.tag=0,((_tmp8F5.f1=Cyc_set_stop_after_objectfile,
_tmp8F5)))),_tmp8F4)))),((_tmp8F8->f5=((_tmp8F3="Produce an object file instead of an executable; do not link",
_tag_dyneither(_tmp8F3,sizeof(char),61))),_tmp8F8)))))))))))),((_tmp7AE[8]=((
_tmp8FE=_region_malloc(r,sizeof(*_tmp8FE)),((_tmp8FE->f1=((_tmp8FD="-m",
_tag_dyneither(_tmp8FD,sizeof(char),3))),((_tmp8FE->f2=1,((_tmp8FE->f3=((_tmp8FC="<option>",
_tag_dyneither(_tmp8FC,sizeof(char),9))),((_tmp8FE->f4=(void*)((_tmp8FA=
_region_malloc(r,sizeof(*_tmp8FA)),((_tmp8FA[0]=((_tmp8FB.tag=1,((_tmp8FB.f1=Cyc_add_marg,
_tmp8FB)))),_tmp8FA)))),((_tmp8FE->f5=((_tmp8F9="GCC specific: pass machine-dependent flag on to gcc",
_tag_dyneither(_tmp8F9,sizeof(char),52))),_tmp8FE)))))))))))),((_tmp7AE[7]=((
_tmp904=_region_malloc(r,sizeof(*_tmp904)),((_tmp904->f1=((_tmp903="-l",
_tag_dyneither(_tmp903,sizeof(char),3))),((_tmp904->f2=1,((_tmp904->f3=((_tmp902="<libname>",
_tag_dyneither(_tmp902,sizeof(char),10))),((_tmp904->f4=(void*)((_tmp900=
_region_malloc(r,sizeof(*_tmp900)),((_tmp900[0]=((_tmp901.tag=1,((_tmp901.f1=Cyc_add_libarg,
_tmp901)))),_tmp900)))),((_tmp904->f5=((_tmp8FF="Library file",_tag_dyneither(
_tmp8FF,sizeof(char),13))),_tmp904)))))))))))),((_tmp7AE[6]=((_tmp90A=
_region_malloc(r,sizeof(*_tmp90A)),((_tmp90A->f1=((_tmp909="-L",_tag_dyneither(
_tmp909,sizeof(char),3))),((_tmp90A->f2=1,((_tmp90A->f3=((_tmp908="<dir>",
_tag_dyneither(_tmp908,sizeof(char),6))),((_tmp90A->f4=(void*)((_tmp906=
_region_malloc(r,sizeof(*_tmp906)),((_tmp906[0]=((_tmp907.tag=1,((_tmp907.f1=Cyc_add_ccarg,
_tmp907)))),_tmp906)))),((_tmp90A->f5=((_tmp905="Add to the list of directories for -l",
_tag_dyneither(_tmp905,sizeof(char),38))),_tmp90A)))))))))))),((_tmp7AE[5]=((
_tmp910=_region_malloc(r,sizeof(*_tmp910)),((_tmp910->f1=((_tmp90F="-I",
_tag_dyneither(_tmp90F,sizeof(char),3))),((_tmp910->f2=1,((_tmp910->f3=((_tmp90E="<dir>",
_tag_dyneither(_tmp90E,sizeof(char),6))),((_tmp910->f4=(void*)((_tmp90C=
_region_malloc(r,sizeof(*_tmp90C)),((_tmp90C[0]=((_tmp90D.tag=1,((_tmp90D.f1=Cyc_add_cpparg,
_tmp90D)))),_tmp90C)))),((_tmp910->f5=((_tmp90B="Add to the list of directories to search for include files",
_tag_dyneither(_tmp90B,sizeof(char),59))),_tmp910)))))))))))),((_tmp7AE[4]=((
_tmp916=_region_malloc(r,sizeof(*_tmp916)),((_tmp916->f1=((_tmp915="-B",
_tag_dyneither(_tmp915,sizeof(char),3))),((_tmp916->f2=1,((_tmp916->f3=((_tmp914="<file>",
_tag_dyneither(_tmp914,sizeof(char),7))),((_tmp916->f4=(void*)((_tmp912=
_region_malloc(r,sizeof(*_tmp912)),((_tmp912[0]=((_tmp913.tag=1,((_tmp913.f1=Cyc_add_cyclone_exec_path,
_tmp913)))),_tmp912)))),((_tmp916->f5=((_tmp911="Add to the list of directories to search for compiler files",
_tag_dyneither(_tmp911,sizeof(char),60))),_tmp916)))))))))))),((_tmp7AE[3]=((
_tmp91C=_region_malloc(r,sizeof(*_tmp91C)),((_tmp91C->f1=((_tmp91B="-D",
_tag_dyneither(_tmp91B,sizeof(char),3))),((_tmp91C->f2=1,((_tmp91C->f3=((_tmp91A="<name>[=<value>]",
_tag_dyneither(_tmp91A,sizeof(char),17))),((_tmp91C->f4=(void*)((_tmp918=
_region_malloc(r,sizeof(*_tmp918)),((_tmp918[0]=((_tmp919.tag=1,((_tmp919.f1=Cyc_add_cpparg,
_tmp919)))),_tmp918)))),((_tmp91C->f5=((_tmp917="Pass definition to preprocessor",
_tag_dyneither(_tmp917,sizeof(char),32))),_tmp91C)))))))))))),((_tmp7AE[2]=((
_tmp922=_region_malloc(r,sizeof(*_tmp922)),((_tmp922->f1=((_tmp921="-o",
_tag_dyneither(_tmp921,sizeof(char),3))),((_tmp922->f2=0,((_tmp922->f3=((_tmp920=" <file>",
_tag_dyneither(_tmp920,sizeof(char),8))),((_tmp922->f4=(void*)((_tmp91E=
_region_malloc(r,sizeof(*_tmp91E)),((_tmp91E[0]=((_tmp91F.tag=5,((_tmp91F.f1=Cyc_set_output_file,
_tmp91F)))),_tmp91E)))),((_tmp922->f5=((_tmp91D="Set the output file name to <file>",
_tag_dyneither(_tmp91D,sizeof(char),35))),_tmp922)))))))))))),((_tmp7AE[1]=((
_tmp928=_region_malloc(r,sizeof(*_tmp928)),((_tmp928->f1=((_tmp927="--version",
_tag_dyneither(_tmp927,sizeof(char),10))),((_tmp928->f2=0,((_tmp928->f3=((
_tmp926="",_tag_dyneither(_tmp926,sizeof(char),1))),((_tmp928->f4=(void*)((
_tmp924=_region_malloc(r,sizeof(*_tmp924)),((_tmp924[0]=((_tmp925.tag=0,((
_tmp925.f1=Cyc_print_version,_tmp925)))),_tmp924)))),((_tmp928->f5=((_tmp923="Print version information and exit",
_tag_dyneither(_tmp923,sizeof(char),35))),_tmp928)))))))))))),((_tmp7AE[0]=((
_tmp92E=_region_malloc(r,sizeof(*_tmp92E)),((_tmp92E->f1=((_tmp92D="-v",
_tag_dyneither(_tmp92D,sizeof(char),3))),((_tmp92E->f2=0,((_tmp92E->f3=((_tmp92C="",
_tag_dyneither(_tmp92C,sizeof(char),1))),((_tmp92E->f4=(void*)((_tmp92A=
_region_malloc(r,sizeof(*_tmp92A)),((_tmp92A[0]=((_tmp92B.tag=3,((_tmp92B.f1=&
Cyc_v_r,_tmp92B)))),_tmp92A)))),((_tmp92E->f5=((_tmp929="Print compilation stages verbosely",
_tag_dyneither(_tmp929,sizeof(char),35))),_tmp92E)))))))))))),((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r,_tag_dyneither(
_tmp7AE,sizeof(struct _tuple22*),64)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))));
const char*_tmp92F;struct _dyneither_ptr optstring=(_tmp92F="Options:",
_tag_dyneither(_tmp92F,sizeof(char),9));Cyc_Arg_current=0;Cyc_Arg_parse(options,
Cyc_add_other,optstring,_tmp153);if(Cyc_target_arch.curr == (_tag_dyneither(0,0,0)).curr)
Cyc_target_arch=(struct _dyneither_ptr)Cstring_to_string(Carch);{char*_tmp930;
struct _dyneither_ptr cyclone_exec_prefix=(_tmp930=getenv((const char*)"CYCLONE_EXEC_PREFIX"),
_tag_dyneither(_tmp930,sizeof(char),_get_zero_arr_size_char(_tmp930,1)));if(
cyclone_exec_prefix.curr != (_tag_dyneither(0,0,0)).curr){struct _dyneither_ptr*
_tmp933;struct Cyc_List_List*_tmp932;Cyc_cyclone_exec_path=((_tmp932=_cycalloc(
sizeof(*_tmp932)),((_tmp932->hd=((_tmp933=_cycalloc(sizeof(*_tmp933)),((_tmp933[
0]=cyclone_exec_prefix,_tmp933)))),((_tmp932->tl=Cyc_cyclone_exec_path,_tmp932))))));}
def_lib_path=(struct _dyneither_ptr)Cstring_to_string(Cdef_lib_path);if(Cyc_strlen((
struct _dyneither_ptr)def_lib_path)> 0){struct _dyneither_ptr*_tmp939;const char*
_tmp938;struct Cyc_List_List*_tmp937;Cyc_cyclone_exec_path=((_tmp937=_cycalloc(
sizeof(*_tmp937)),((_tmp937->hd=((_tmp939=_cycalloc(sizeof(*_tmp939)),((_tmp939[
0]=(struct _dyneither_ptr)Cyc_Filename_concat(def_lib_path,((_tmp938="cyc-lib",
_tag_dyneither(_tmp938,sizeof(char),8)))),_tmp939)))),((_tmp937->tl=Cyc_cyclone_exec_path,
_tmp937))))));}Cyc_cyclone_exec_path=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_cyclone_exec_path);cyclone_arch_path=Cyc_also_subdir(
Cyc_cyclone_exec_path,Cyc_target_arch);{const char*_tmp93A;struct _dyneither_ptr
_tmp15A=Cyc_do_find(cyclone_arch_path,((_tmp93A="cycspecs",_tag_dyneither(
_tmp93A,sizeof(char),9))));if(Cyc_v_r){const char*_tmp93E;void*_tmp93D[1];struct
Cyc_String_pa_struct _tmp93C;(_tmp93C.tag=0,((_tmp93C.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp15A),((_tmp93D[0]=& _tmp93C,Cyc_fprintf(Cyc_stderr,((
_tmp93E="Reading from specs file %s\n",_tag_dyneither(_tmp93E,sizeof(char),28))),
_tag_dyneither(_tmp93D,sizeof(void*),1)))))));}{struct Cyc_List_List*_tmp15E=Cyc_read_specs(
_tmp15A);struct _dyneither_ptr _tmp15F=_tag_dyneither(0,0,0);{struct _handler_cons
_tmp160;_push_handler(& _tmp160);{int _tmp162=0;if(setjmp(_tmp160.handler))_tmp162=
1;if(!_tmp162){{struct _dyneither_ptr _tmp163=*((struct _dyneither_ptr*(*)(int(*cmp)(
struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_assoc_cmp)(Cyc_strptrcmp,_tmp15E,_init_dyneither_ptr(
_cycalloc(sizeof(struct _dyneither_ptr)),"cyclone",sizeof(char),8));_tmp15F=Cyc_split_specs(
_tmp163);};_pop_handler();}else{void*_tmp161=(void*)_exn_thrown;void*_tmp166=
_tmp161;_LL36: {struct Cyc_Core_Not_found_struct*_tmp167=(struct Cyc_Core_Not_found_struct*)
_tmp166;if(_tmp167->tag != Cyc_Core_Not_found)goto _LL38;}_LL37: goto _LL35;_LL38:;
_LL39:(void)_throw(_tmp166);_LL35:;}};}if(_tmp15F.curr != (_tag_dyneither(0,0,0)).curr){
Cyc_Arg_current=0;Cyc_Arg_parse(options,Cyc_add_other,optstring,_tmp15F);}Cyc_Arg_current=
0;Cyc_Arg_parse(options,Cyc_add_other,optstring,_tmp154);{const char*_tmp93F;if(
Cyc_strcmp((struct _dyneither_ptr)Cyc_cpp,((_tmp93F="",_tag_dyneither(_tmp93F,
sizeof(char),1))))== 0){const char*_tmp944;void*_tmp943[2];struct Cyc_String_pa_struct
_tmp942;struct Cyc_String_pa_struct _tmp941;Cyc_set_cpp((struct _dyneither_ptr)((
_tmp941.tag=0,((_tmp941.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp15A),((
_tmp942.tag=0,((_tmp942.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)comp),((
_tmp943[0]=& _tmp942,((_tmp943[1]=& _tmp941,Cyc_aprintf(((_tmp944="%s -w -x c -E -specs %s",
_tag_dyneither(_tmp944,sizeof(char),24))),_tag_dyneither(_tmp943,sizeof(void*),2)))))))))))))));}};};};};};};}
if(Cyc_cyclone_files == 0  && Cyc_ccargs == 0){{const char*_tmp947;void*_tmp946;(
_tmp946=0,Cyc_fprintf(Cyc_stderr,((_tmp947="missing file\n",_tag_dyneither(
_tmp947,sizeof(char),14))),_tag_dyneither(_tmp946,sizeof(void*),0)));}exit(1);}
if(!Cyc_stop_after_cpp_r  && !Cyc_nocyc_setjmp_r){const char*_tmp948;Cyc_cyc_setjmp=
Cyc_do_find(cyclone_arch_path,((_tmp948="cyc_setjmp.h",_tag_dyneither(_tmp948,
sizeof(char),13))));}if(!Cyc_stop_after_cpp_r  && Cyc_strlen((struct
_dyneither_ptr)Cyc_cyc_include)== 0){const char*_tmp949;Cyc_cyc_include=Cyc_do_find(
Cyc_cyclone_exec_path,((_tmp949="cyc_include.h",_tag_dyneither(_tmp949,sizeof(
char),14))));}{struct Cyc_List_List*_tmp301=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(Cyc_cyclone_files);for(0;_tmp301 != 0;_tmp301=_tmp301->tl){Cyc_process_file(*((
struct _dyneither_ptr*)_tmp301->hd));if(Cyc_compile_failure){int _tmp302=1;
_npop_handler(0);return _tmp302;}}}if(((Cyc_stop_after_cpp_r  || Cyc_parseonly_r)
 || Cyc_tc_r) || Cyc_toc_r){int _tmp303=0;_npop_handler(0);return _tmp303;}if(Cyc_ccargs
== 0){int _tmp304=0;_npop_handler(0);return _tmp304;}{const char*_tmp94A;Cyc_add_ccarg((
struct _dyneither_ptr)Cyc_strconcat(((_tmp94A="-L",_tag_dyneither(_tmp94A,sizeof(
char),3))),(struct _dyneither_ptr)def_lib_path));}Cyc_ccargs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{const char*_tmp94B;struct
_dyneither_ptr _tmp306=Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct
_dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,Cyc_ccargs),((_tmp94B=" ",_tag_dyneither(_tmp94B,sizeof(
char),2))));Cyc_libargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_libargs);{const char*_tmp94D;struct Cyc_List_List*_tmp94C;struct _dyneither_ptr
_tmp307=Cyc_str_sepstr(((_tmp94C=_cycalloc(sizeof(*_tmp94C)),((_tmp94C->hd=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),((
_tmp94C->tl=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,
Cyc_libargs),_tmp94C)))))),((_tmp94D=" ",_tag_dyneither(_tmp94D,sizeof(char),2))));
struct Cyc_List_List*stdlib;struct _dyneither_ptr stdlib_string;const char*_tmp94F;
const char*_tmp94E;int _tmp308=((Cyc_stop_after_asmfile_r  || Cyc_stop_after_objectfile_r)
 || Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)
_check_null(Cyc_output_file)),((_tmp94F=".a",_tag_dyneither(_tmp94F,sizeof(char),
3))))) || Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct
_dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp94E=".lib",_tag_dyneither(
_tmp94E,sizeof(char),5))));if(_tmp308){stdlib=0;{const char*_tmp950;stdlib_string=(
struct _dyneither_ptr)((_tmp950="",_tag_dyneither(_tmp950,sizeof(char),1)));};}
else{struct _dyneither_ptr libcyc_flag;struct _dyneither_ptr nogc_filename;struct
_dyneither_ptr runtime_filename;struct _dyneither_ptr gc_filename;if(Cyc_pa_r){{
const char*_tmp951;libcyc_flag=((_tmp951="-lcyc_a",_tag_dyneither(_tmp951,sizeof(
char),8)));}{const char*_tmp952;nogc_filename=((_tmp952="nogc_a.a",_tag_dyneither(
_tmp952,sizeof(char),9)));}{const char*_tmp953;runtime_filename=((_tmp953="runtime_cyc_a.o",
_tag_dyneither(_tmp953,sizeof(char),16)));};}else{if(Cyc_nocheck_r){{const char*
_tmp954;libcyc_flag=((_tmp954="-lcyc_nocheck",_tag_dyneither(_tmp954,sizeof(char),
14)));}{const char*_tmp955;nogc_filename=((_tmp955="nogc_nocheck.a",
_tag_dyneither(_tmp955,sizeof(char),15)));}{const char*_tmp956;runtime_filename=((
_tmp956="runtime_cyc.o",_tag_dyneither(_tmp956,sizeof(char),14)));};}else{if(Cyc_pg_r){{
const char*_tmp957;libcyc_flag=((_tmp957="-lcyc_pg",_tag_dyneither(_tmp957,
sizeof(char),9)));}{const char*_tmp958;runtime_filename=((_tmp958="runtime_cyc_pg.o",
_tag_dyneither(_tmp958,sizeof(char),17)));}{const char*_tmp959;nogc_filename=((
_tmp959="nogc_pg.a",_tag_dyneither(_tmp959,sizeof(char),10)));};}else{{const char*
_tmp95A;libcyc_flag=((_tmp95A="-lcyc",_tag_dyneither(_tmp95A,sizeof(char),6)));}{
const char*_tmp95B;nogc_filename=((_tmp95B="nogc.a",_tag_dyneither(_tmp95B,
sizeof(char),7)));}{const char*_tmp95C;runtime_filename=((_tmp95C="runtime_cyc.o",
_tag_dyneither(_tmp95C,sizeof(char),14)));};}}}{const char*_tmp95D;gc_filename=((
_tmp95D="gc.a",_tag_dyneither(_tmp95D,sizeof(char),5)));}{struct _dyneither_ptr
_tmp317=Cyc_nogc_r?Cyc_do_find(cyclone_arch_path,nogc_filename): Cyc_do_find(
cyclone_arch_path,gc_filename);struct _dyneither_ptr _tmp318=Cyc_do_find(
cyclone_arch_path,runtime_filename);stdlib=0;{const char*_tmp963;void*_tmp962[3];
struct Cyc_String_pa_struct _tmp961;struct Cyc_String_pa_struct _tmp960;struct Cyc_String_pa_struct
_tmp95F;stdlib_string=(struct _dyneither_ptr)((_tmp95F.tag=0,((_tmp95F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp317),((_tmp960.tag=0,((_tmp960.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)libcyc_flag),((_tmp961.tag=0,((
_tmp961.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp318),((_tmp962[0]=&
_tmp961,((_tmp962[1]=& _tmp960,((_tmp962[2]=& _tmp95F,Cyc_aprintf(((_tmp963=" %s %s %s",
_tag_dyneither(_tmp963,sizeof(char),10))),_tag_dyneither(_tmp962,sizeof(void*),3))))))))))))))))))));};};}
if(Cyc_ic_r){struct _handler_cons _tmp31E;_push_handler(& _tmp31E);{int _tmp320=0;
if(setjmp(_tmp31E.handler))_tmp320=1;if(!_tmp320){Cyc_ccargs=((struct Cyc_List_List*(*)(
int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,
Cyc_ccargs);Cyc_libargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),
struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{struct Cyc_List_List*
_tmp321=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(stdlib,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));if(!_tmp308){struct
Cyc_List_List*_tmp964;_tmp321=((_tmp964=_cycalloc(sizeof(*_tmp964)),((_tmp964->hd=
Cyc_final_strptr,((_tmp964->tl=_tmp321,_tmp964))))));}{struct Cyc_Interface_I*
_tmp323=((struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*get)(struct
_dyneither_ptr*),struct Cyc_List_List*la,struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(
Cyc_read_cycio,_tmp321,_tmp321);if(_tmp323 == 0){{const char*_tmp967;void*_tmp966;(
_tmp966=0,Cyc_fprintf(Cyc_stderr,((_tmp967="Error: interfaces incompatible\n",
_tag_dyneither(_tmp967,sizeof(char),32))),_tag_dyneither(_tmp966,sizeof(void*),0)));}
Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp326=1;_npop_handler(1);return
_tmp326;};}if(_tmp308){if(Cyc_output_file != 0){const char*_tmp96B;void*_tmp96A[1];
struct Cyc_String_pa_struct _tmp969;struct _dyneither_ptr _tmp327=(_tmp969.tag=0,((
_tmp969.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Filename_chop_extension(*((
struct _dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmp96A[0]=& _tmp969,Cyc_aprintf(((
_tmp96B="%s.cycio",_tag_dyneither(_tmp96B,sizeof(char),9))),_tag_dyneither(
_tmp96A,sizeof(void*),1)))))));const char*_tmp96D;const char*_tmp96C;struct Cyc___cycFILE*
_tmp328=Cyc_try_file_open((struct _dyneither_ptr)_tmp327,((_tmp96C="wb",
_tag_dyneither(_tmp96C,sizeof(char),3))),((_tmp96D="interface object file",
_tag_dyneither(_tmp96D,sizeof(char),22))));if(_tmp328 == 0){Cyc_compile_failure=1;
Cyc_remove_cfiles();{int _tmp329=1;_npop_handler(1);return _tmp329;};}Cyc_Interface_save((
struct Cyc_Interface_I*)_tmp323,(struct Cyc___cycFILE*)_tmp328);Cyc_file_close((
struct Cyc___cycFILE*)_tmp328);}}else{const char*_tmp972;const char*_tmp971;struct
_tuple18*_tmp970;if(!Cyc_Interface_is_subinterface(Cyc_Interface_empty(),(struct
Cyc_Interface_I*)_tmp323,((_tmp970=_cycalloc(sizeof(*_tmp970)),((_tmp970->f1=((
_tmp971="empty interface",_tag_dyneither(_tmp971,sizeof(char),16))),((_tmp970->f2=((
_tmp972="global interface",_tag_dyneither(_tmp972,sizeof(char),17))),_tmp970)))))))){{
const char*_tmp975;void*_tmp974;(_tmp974=0,Cyc_fprintf(Cyc_stderr,((_tmp975="Error: some objects are still undefined\n",
_tag_dyneither(_tmp975,sizeof(char),41))),_tag_dyneither(_tmp974,sizeof(void*),0)));}
Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp334=1;_npop_handler(1);return
_tmp334;};}}};};;_pop_handler();}else{void*_tmp31F=(void*)_exn_thrown;void*
_tmp336=_tmp31F;_LL3B:;_LL3C:{void*_tmp337=_tmp336;struct _dyneither_ptr _tmp339;
struct _dyneither_ptr _tmp33B;struct _dyneither_ptr _tmp33E;_LL40: {struct Cyc_Core_Failure_struct*
_tmp338=(struct Cyc_Core_Failure_struct*)_tmp337;if(_tmp338->tag != Cyc_Core_Failure)
goto _LL42;else{_tmp339=_tmp338->f1;}}_LL41:{const char*_tmp979;void*_tmp978[1];
struct Cyc_String_pa_struct _tmp977;(_tmp977.tag=0,((_tmp977.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp339),((_tmp978[0]=& _tmp977,Cyc_fprintf(
Cyc_stderr,((_tmp979="Exception Core::Failure %s\n",_tag_dyneither(_tmp979,
sizeof(char),28))),_tag_dyneither(_tmp978,sizeof(void*),1)))))));}goto _LL3F;
_LL42: {struct Cyc_Core_Impossible_struct*_tmp33A=(struct Cyc_Core_Impossible_struct*)
_tmp337;if(_tmp33A->tag != Cyc_Core_Impossible)goto _LL44;else{_tmp33B=_tmp33A->f1;}}
_LL43:{const char*_tmp97D;void*_tmp97C[1];struct Cyc_String_pa_struct _tmp97B;(
_tmp97B.tag=0,((_tmp97B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp33B),((
_tmp97C[0]=& _tmp97B,Cyc_fprintf(Cyc_stderr,((_tmp97D="Exception Core::Impossible %s\n",
_tag_dyneither(_tmp97D,sizeof(char),31))),_tag_dyneither(_tmp97C,sizeof(void*),1)))))));}
goto _LL3F;_LL44: {struct Cyc_Dict_Absent_struct*_tmp33C=(struct Cyc_Dict_Absent_struct*)
_tmp337;if(_tmp33C->tag != Cyc_Dict_Absent)goto _LL46;}_LL45:{const char*_tmp980;
void*_tmp97F;(_tmp97F=0,Cyc_fprintf(Cyc_stderr,((_tmp980="Exception Dict::Absent\n",
_tag_dyneither(_tmp980,sizeof(char),24))),_tag_dyneither(_tmp97F,sizeof(void*),0)));}
goto _LL3F;_LL46: {struct Cyc_Core_Invalid_argument_struct*_tmp33D=(struct Cyc_Core_Invalid_argument_struct*)
_tmp337;if(_tmp33D->tag != Cyc_Core_Invalid_argument)goto _LL48;else{_tmp33E=
_tmp33D->f1;}}_LL47:{const char*_tmp984;void*_tmp983[1];struct Cyc_String_pa_struct
_tmp982;(_tmp982.tag=0,((_tmp982.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp33E),((_tmp983[0]=& _tmp982,Cyc_fprintf(Cyc_stderr,((_tmp984="Exception Core::Invalid_argument %s\n",
_tag_dyneither(_tmp984,sizeof(char),37))),_tag_dyneither(_tmp983,sizeof(void*),1)))))));}
goto _LL3F;_LL48:;_LL49:{const char*_tmp987;void*_tmp986;(_tmp986=0,Cyc_fprintf(
Cyc_stderr,((_tmp987="Uncaught exception\n",_tag_dyneither(_tmp987,sizeof(char),
20))),_tag_dyneither(_tmp986,sizeof(void*),0)));}goto _LL3F;_LL3F:;}Cyc_compile_failure=
1;Cyc_remove_cfiles();{int _tmp34C=1;_npop_handler(0);return _tmp34C;};_LL3D:;
_LL3E:(void)_throw(_tmp336);_LL3A:;}};}{const char*_tmp988;struct _dyneither_ptr
outfile_str=(_tmp988="",_tag_dyneither(_tmp988,sizeof(char),1));if(Cyc_output_file
!= 0){const char*_tmp98C;void*_tmp98B[1];struct Cyc_String_pa_struct _tmp98A;
outfile_str=(struct _dyneither_ptr)((_tmp98A.tag=0,((_tmp98A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(*((struct
_dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmp98B[0]=& _tmp98A,Cyc_aprintf(((
_tmp98C=" -o %s",_tag_dyneither(_tmp98C,sizeof(char),7))),_tag_dyneither(_tmp98B,
sizeof(void*),1))))))));}{const char*_tmp994;void*_tmp993[5];struct Cyc_String_pa_struct
_tmp992;struct Cyc_String_pa_struct _tmp991;struct Cyc_String_pa_struct _tmp990;
struct Cyc_String_pa_struct _tmp98F;struct Cyc_String_pa_struct _tmp98E;struct
_dyneither_ptr _tmp350=(_tmp98E.tag=0,((_tmp98E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp307),((_tmp98F.tag=0,((_tmp98F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)stdlib_string),((_tmp990.tag=0,((_tmp990.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp306),((_tmp991.tag=0,((_tmp991.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)outfile_str),((_tmp992.tag=0,((
_tmp992.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)comp),((_tmp993[0]=&
_tmp992,((_tmp993[1]=& _tmp991,((_tmp993[2]=& _tmp990,((_tmp993[3]=& _tmp98F,((
_tmp993[4]=& _tmp98E,Cyc_aprintf(((_tmp994="%s %s %s%s%s",_tag_dyneither(_tmp994,
sizeof(char),13))),_tag_dyneither(_tmp993,sizeof(void*),5)))))))))))))))))))))))))))))));
if(Cyc_v_r){{const char*_tmp998;void*_tmp997[1];struct Cyc_String_pa_struct _tmp996;(
_tmp996.tag=0,((_tmp996.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp350),((
_tmp997[0]=& _tmp996,Cyc_fprintf(Cyc_stderr,((_tmp998="%s\n",_tag_dyneither(
_tmp998,sizeof(char),4))),_tag_dyneither(_tmp997,sizeof(void*),1)))))));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);}if(system((const char*)_untag_dyneither_ptr(
_tmp350,sizeof(char),1))!= 0){{const char*_tmp99B;void*_tmp99A;(_tmp99A=0,Cyc_fprintf(
Cyc_stderr,((_tmp99B="Error: C compiler failed\n",_tag_dyneither(_tmp99B,sizeof(
char),26))),_tag_dyneither(_tmp99A,sizeof(void*),0)));}Cyc_compile_failure=1;Cyc_remove_cfiles();{
int _tmp356=1;_npop_handler(0);return _tmp356;};}Cyc_remove_cfiles();{int _tmp357=
Cyc_compile_failure?1: 0;_npop_handler(0);return _tmp357;};};};};};;_pop_region(r);};}
