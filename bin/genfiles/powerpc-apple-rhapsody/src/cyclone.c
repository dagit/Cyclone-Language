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
_dyneither_ptr s){struct _dyneither_ptr*_tmp36B;struct Cyc_List_List*_tmp36A;Cyc_ccargs=((
_tmp36A=_cycalloc(sizeof(*_tmp36A)),((_tmp36A->hd=((_tmp36B=_cycalloc(sizeof(*
_tmp36B)),((_tmp36B[0]=s,_tmp36B)))),((_tmp36A->tl=Cyc_ccargs,_tmp36A))))));}
void Cyc_set_c_compiler(struct _dyneither_ptr s);void Cyc_set_c_compiler(struct
_dyneither_ptr s){const char*_tmp36C;if(Cyc_strcmp((struct _dyneither_ptr)s,((
_tmp36C="vc",_tag_dyneither(_tmp36C,sizeof(char),3))))== 0){Cyc_Cyclone_c_compiler=
Cyc_Cyclone_Vc_c;{const char*_tmp36D;Cyc_add_ccarg(((_tmp36D="-DVC_C",
_tag_dyneither(_tmp36D,sizeof(char),7))));};}else{const char*_tmp36E;if(Cyc_strcmp((
struct _dyneither_ptr)s,((_tmp36E="gcc",_tag_dyneither(_tmp36E,sizeof(char),4))))
== 0){Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;{const char*_tmp36F;Cyc_add_ccarg(((
_tmp36F="-DGCC_C",_tag_dyneither(_tmp36F,sizeof(char),8))));};}}}void Cyc_set_port_c_code();
void Cyc_set_port_c_code(){Cyc_Absyn_porting_c_code=1;Cyc_Position_max_errors=
65535;Cyc_save_c_r=1;Cyc_parseonly_r=1;}static struct _dyneither_ptr*Cyc_output_file=
0;static void Cyc_set_output_file(struct _dyneither_ptr s);static void Cyc_set_output_file(
struct _dyneither_ptr s){struct _dyneither_ptr*_tmp370;Cyc_output_file=((_tmp370=
_cycalloc(sizeof(*_tmp370)),((_tmp370[0]=s,_tmp370))));}extern char*Cdef_inc_path;
extern char*Cdef_lib_path;extern char*Carch;extern char*Ccomp;extern char*Cversion;
static char _tmp7[1]="";static struct _dyneither_ptr Cyc_cpp={_tmp7,_tmp7,_tmp7 + 1};
static void Cyc_set_cpp(struct _dyneither_ptr s);static void Cyc_set_cpp(struct
_dyneither_ptr s){Cyc_cpp=s;}static struct Cyc_List_List*Cyc_cppargs=0;static void
Cyc_add_cpparg(struct _dyneither_ptr s);static void Cyc_add_cpparg(struct
_dyneither_ptr s){struct _dyneither_ptr*_tmp373;struct Cyc_List_List*_tmp372;Cyc_cppargs=((
_tmp372=_cycalloc(sizeof(*_tmp372)),((_tmp372->hd=((_tmp373=_cycalloc(sizeof(*
_tmp373)),((_tmp373[0]=s,_tmp373)))),((_tmp372->tl=Cyc_cppargs,_tmp372))))));}
static struct _dyneither_ptr Cyc_target_arch=(struct _dyneither_ptr){(void*)0,(void*)
0,(void*)(0 + 0)};static void Cyc_set_target_arch(struct _dyneither_ptr s);static void
Cyc_set_target_arch(struct _dyneither_ptr s){Cyc_toc_r=1;Cyc_target_arch=s;}static
void Cyc_print_version();static void Cyc_print_version(){{const char*_tmp377;void*
_tmp376[1];struct Cyc_String_pa_struct _tmp375;(_tmp375.tag=0,((_tmp375.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cversion)),((_tmp376[0]=&
_tmp375,Cyc_printf(((_tmp377="The Cyclone compiler, version %s\n",_tag_dyneither(
_tmp377,sizeof(char),34))),_tag_dyneither(_tmp376,sizeof(void*),1)))))));}{const
char*_tmp37C;void*_tmp37B[2];struct Cyc_String_pa_struct _tmp37A;struct Cyc_String_pa_struct
_tmp379;(_tmp379.tag=0,((_tmp379.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cstring_to_string(Ccomp)),((_tmp37A.tag=0,((_tmp37A.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cstring_to_string(Carch)),((_tmp37B[0]=& _tmp37A,((_tmp37B[1]=&
_tmp379,Cyc_printf(((_tmp37C="Compiled for architecture: %s CC=\"%s\"\n",
_tag_dyneither(_tmp37C,sizeof(char),39))),_tag_dyneither(_tmp37B,sizeof(void*),2)))))))))))));}{
const char*_tmp380;void*_tmp37F[1];struct Cyc_String_pa_struct _tmp37E;(_tmp37E.tag=
0,((_tmp37E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(
Cdef_lib_path)),((_tmp37F[0]=& _tmp37E,Cyc_printf(((_tmp380="Standard library directory: %s\n",
_tag_dyneither(_tmp380,sizeof(char),32))),_tag_dyneither(_tmp37F,sizeof(void*),1)))))));}{
const char*_tmp384;void*_tmp383[1];struct Cyc_String_pa_struct _tmp382;(_tmp382.tag=
0,((_tmp382.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(
Cdef_inc_path)),((_tmp383[0]=& _tmp382,Cyc_printf(((_tmp384="Standard include directory: %s\n",
_tag_dyneither(_tmp384,sizeof(char),32))),_tag_dyneither(_tmp383,sizeof(void*),1)))))));}
exit(0);}static int Cyc_is_cyclone_sourcefile(struct _dyneither_ptr s);static int Cyc_is_cyclone_sourcefile(
struct _dyneither_ptr s){unsigned long _tmp17=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp17 <= 4)return 0;else{const char*_tmp385;return Cyc_strcmp((struct
_dyneither_ptr)_dyneither_ptr_plus(s,sizeof(char),(int)(_tmp17 - 4)),((_tmp385=".cyc",
_tag_dyneither(_tmp385,sizeof(char),5))))== 0;}}static struct Cyc_List_List*Cyc_cyclone_exec_path=
0;static void Cyc_add_cyclone_exec_path(struct _dyneither_ptr s);static void Cyc_add_cyclone_exec_path(
struct _dyneither_ptr s){unsigned long _tmp19=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp19 <= 2)return;{struct _dyneither_ptr _tmp1A=(struct _dyneither_ptr)Cyc_substring((
struct _dyneither_ptr)s,2,_tmp19 - 2);struct _dyneither_ptr*_tmp388;struct Cyc_List_List*
_tmp387;Cyc_cyclone_exec_path=((_tmp387=_cycalloc(sizeof(*_tmp387)),((_tmp387->hd=((
_tmp388=_cycalloc(sizeof(*_tmp388)),((_tmp388[0]=_tmp1A,_tmp388)))),((_tmp387->tl=
Cyc_cyclone_exec_path,_tmp387))))));};}static struct Cyc_List_List*Cyc_libargs=0;
static void Cyc_add_libarg(struct _dyneither_ptr s);static void Cyc_add_libarg(struct
_dyneither_ptr s){const char*_tmp389;if(Cyc_strcmp((struct _dyneither_ptr)s,((
_tmp389="-lxml",_tag_dyneither(_tmp389,sizeof(char),6))))== 0)Cyc_add_ccarg(s);
else{struct _dyneither_ptr*_tmp38C;struct Cyc_List_List*_tmp38B;Cyc_libargs=((
_tmp38B=_cycalloc(sizeof(*_tmp38B)),((_tmp38B->hd=((_tmp38C=_cycalloc(sizeof(*
_tmp38C)),((_tmp38C[0]=s,_tmp38C)))),((_tmp38B->tl=Cyc_libargs,_tmp38B))))));}}
static void Cyc_add_marg(struct _dyneither_ptr s);static void Cyc_add_marg(struct
_dyneither_ptr s){Cyc_add_ccarg(s);}static void Cyc_set_save_temps();static void Cyc_set_save_temps(){
Cyc_save_temps_r=1;{const char*_tmp38D;Cyc_add_ccarg(((_tmp38D="-save-temps",
_tag_dyneither(_tmp38D,sizeof(char),12))));};}static int Cyc_produce_dependencies=
0;static void Cyc_set_produce_dependencies();static void Cyc_set_produce_dependencies(){
Cyc_stop_after_cpp_r=1;Cyc_produce_dependencies=1;{const char*_tmp38E;Cyc_add_cpparg(((
_tmp38E="-M",_tag_dyneither(_tmp38E,sizeof(char),3))));};}static struct
_dyneither_ptr*Cyc_dependencies_target=0;static void Cyc_set_dependencies_target(
struct _dyneither_ptr s);static void Cyc_set_dependencies_target(struct
_dyneither_ptr s){struct _dyneither_ptr*_tmp38F;Cyc_dependencies_target=((_tmp38F=
_cycalloc(sizeof(*_tmp38F)),((_tmp38F[0]=s,_tmp38F))));}static void Cyc_set_stop_after_objectfile();
static void Cyc_set_stop_after_objectfile(){Cyc_stop_after_objectfile_r=1;{const
char*_tmp390;Cyc_add_ccarg(((_tmp390="-c",_tag_dyneither(_tmp390,sizeof(char),3))));};}
static void Cyc_set_nocppprecomp();static void Cyc_set_nocppprecomp(){{const char*
_tmp391;Cyc_add_cpparg(((_tmp391="-no-cpp-precomp",_tag_dyneither(_tmp391,
sizeof(char),16))));}{const char*_tmp392;Cyc_add_ccarg(((_tmp392="-no-cpp-precomp",
_tag_dyneither(_tmp392,sizeof(char),16))));};}static void Cyc_set_lineno();static
void Cyc_set_lineno(){Cyc_generate_line_directives_r=1;Cyc_set_save_temps();}
static void Cyc_set_inline_functions();static void Cyc_set_inline_functions(){Cyc_inline_functions_r=
1;}static void Cyc_set_elim_se();static void Cyc_set_elim_se(){Cyc_elim_se_r=1;Cyc_set_inline_functions();}
static void Cyc_set_tovc();static void Cyc_set_tovc(){Cyc_Cyclone_tovc_r=1;{const
char*_tmp393;Cyc_add_ccarg(((_tmp393="-DCYC_ANSI_OUTPUT",_tag_dyneither(_tmp393,
sizeof(char),18))));}Cyc_set_elim_se();}static void Cyc_set_noboundschecks();
static void Cyc_set_noboundschecks(){const char*_tmp394;Cyc_add_ccarg(((_tmp394="-DNO_CYC_BOUNDS_CHECKS",
_tag_dyneither(_tmp394,sizeof(char),23))));}static void Cyc_set_nonullchecks();
static void Cyc_set_nonullchecks(){const char*_tmp395;Cyc_add_ccarg(((_tmp395="-DNO_CYC_NULL_CHECKS",
_tag_dyneither(_tmp395,sizeof(char),21))));}static void Cyc_set_nochecks();static
void Cyc_set_nochecks(){Cyc_set_noboundschecks();Cyc_set_nonullchecks();Cyc_nocheck_r=
1;}static void Cyc_set_nocyc();static void Cyc_set_nocyc(){Cyc_add_cyc_namespace_r=0;{
const char*_tmp396;Cyc_add_ccarg(((_tmp396="-DNO_CYC_PREFIX",_tag_dyneither(
_tmp396,sizeof(char),16))));};}static void Cyc_set_pa();static void Cyc_set_pa(){Cyc_pa_r=
1;{const char*_tmp397;Cyc_add_ccarg(((_tmp397="-DCYC_REGION_PROFILE",
_tag_dyneither(_tmp397,sizeof(char),21))));};}static void Cyc_set_pg();static void
Cyc_set_pg(){Cyc_pg_r=1;{const char*_tmp398;Cyc_add_ccarg(((_tmp398="-pg",
_tag_dyneither(_tmp398,sizeof(char),4))));};}static void Cyc_set_stop_after_asmfile();
static void Cyc_set_stop_after_asmfile(){Cyc_stop_after_asmfile_r=1;{const char*
_tmp399;Cyc_add_ccarg(((_tmp399="-S",_tag_dyneither(_tmp399,sizeof(char),3))));};}
static void Cyc_set_all_warnings();static void Cyc_set_all_warnings(){Cyc_Toc_warn_bounds_checks=
1;Cyc_Toc_warn_all_null_deref=1;}enum Cyc_inputtype{Cyc_DEFAULTINPUT  = 0,Cyc_CYCLONEFILE
 = 1};static enum Cyc_inputtype Cyc_intype=Cyc_DEFAULTINPUT;static void Cyc_set_inputtype(
struct _dyneither_ptr s);static void Cyc_set_inputtype(struct _dyneither_ptr s){const
char*_tmp39A;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp39A="cyc",
_tag_dyneither(_tmp39A,sizeof(char),4))))== 0)Cyc_intype=Cyc_CYCLONEFILE;else{
const char*_tmp39B;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp39B="none",
_tag_dyneither(_tmp39B,sizeof(char),5))))== 0)Cyc_intype=Cyc_DEFAULTINPUT;else{
const char*_tmp39F;void*_tmp39E[1];struct Cyc_String_pa_struct _tmp39D;(_tmp39D.tag=
0,((_tmp39D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp39E[0]=&
_tmp39D,Cyc_fprintf(Cyc_stderr,((_tmp39F="Input type '%s' not supported\n",
_tag_dyneither(_tmp39F,sizeof(char),31))),_tag_dyneither(_tmp39E,sizeof(void*),1)))))));}}}
struct _dyneither_ptr Cyc_make_base_filename(struct _dyneither_ptr s,struct
_dyneither_ptr*output_file);struct _dyneither_ptr Cyc_make_base_filename(struct
_dyneither_ptr s,struct _dyneither_ptr*output_file){struct _dyneither_ptr _tmp32=Cyc_Filename_dirname(
output_file == 0?s:*output_file);struct _dyneither_ptr _tmp33=Cyc_Filename_chop_extension((
struct _dyneither_ptr)Cyc_Filename_basename(s));struct _dyneither_ptr _tmp34=Cyc_strlen((
struct _dyneither_ptr)_tmp32)> 0?Cyc_Filename_concat((struct _dyneither_ptr)_tmp32,(
struct _dyneither_ptr)_tmp33): _tmp33;return(struct _dyneither_ptr)_tmp34;}static
struct Cyc_List_List*Cyc_cyclone_files=0;static void Cyc_add_other(struct
_dyneither_ptr s);static void Cyc_add_other(struct _dyneither_ptr s){if(Cyc_intype == 
Cyc_CYCLONEFILE  || Cyc_is_cyclone_sourcefile(s)){{struct _dyneither_ptr*_tmp3A2;
struct Cyc_List_List*_tmp3A1;Cyc_cyclone_files=((_tmp3A1=_cycalloc(sizeof(*
_tmp3A1)),((_tmp3A1->hd=((_tmp3A2=_cycalloc(sizeof(*_tmp3A2)),((_tmp3A2[0]=s,
_tmp3A2)))),((_tmp3A1->tl=Cyc_cyclone_files,_tmp3A1))))));}{struct _dyneither_ptr
_tmp37=Cyc_make_base_filename(s,Cyc_output_file);const char*_tmp3A3;struct
_dyneither_ptr _tmp38=Cyc_strconcat((struct _dyneither_ptr)_tmp37,((_tmp3A3=".c",
_tag_dyneither(_tmp3A3,sizeof(char),3))));Cyc_add_ccarg((struct _dyneither_ptr)
_tmp38);};}else{Cyc_add_ccarg(s);}}static void Cyc_remove_file(struct
_dyneither_ptr s);static void Cyc_remove_file(struct _dyneither_ptr s){if(Cyc_save_temps_r)
return;else{remove((const char*)_untag_dyneither_ptr(s,sizeof(char),1));}}struct
_tuple19{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;};struct Cyc_List_List*
Cyc_read_specs(struct _dyneither_ptr file);struct Cyc_List_List*Cyc_read_specs(
struct _dyneither_ptr file){struct Cyc_List_List*_tmp3A=0;int c;int i;char strname[256];
char strvalue[4096];struct Cyc___cycFILE*spec_file=Cyc_fopen((const char*)
_untag_dyneither_ptr(file,sizeof(char),1),(const char*)"r");if(spec_file == 0){{
const char*_tmp3A7;void*_tmp3A6[1];struct Cyc_String_pa_struct _tmp3A5;(_tmp3A5.tag=
0,((_tmp3A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp3A6[0]=&
_tmp3A5,Cyc_fprintf(Cyc_stderr,((_tmp3A7="Error opening spec file %s\n",
_tag_dyneither(_tmp3A7,sizeof(char),28))),_tag_dyneither(_tmp3A6,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 0;}while(1){while(1){c=Cyc_fgetc((
struct Cyc___cycFILE*)spec_file);if(isspace(c))continue;if(c == '*')break;if(c != -
1){{const char*_tmp3AC;void*_tmp3AB[2];struct Cyc_String_pa_struct _tmp3AA;struct
Cyc_Int_pa_struct _tmp3A9;(_tmp3A9.tag=1,((_tmp3A9.f1=(unsigned long)c,((_tmp3AA.tag=
0,((_tmp3AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp3AB[0]=&
_tmp3AA,((_tmp3AB[1]=& _tmp3A9,Cyc_fprintf(Cyc_stderr,((_tmp3AC="Error reading spec file %s: unexpected character '%c'\n",
_tag_dyneither(_tmp3AC,sizeof(char),55))),_tag_dyneither(_tmp3AB,sizeof(void*),2)))))))))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}goto CLEANUP_AND_RETURN;}
JUST_AFTER_STAR: i=0;while(1){c=Cyc_fgetc((struct Cyc___cycFILE*)spec_file);if(c == 
- 1){{const char*_tmp3B0;void*_tmp3AF[1];struct Cyc_String_pa_struct _tmp3AE;(
_tmp3AE.tag=0,((_tmp3AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((
_tmp3AF[0]=& _tmp3AE,Cyc_fprintf(Cyc_stderr,((_tmp3B0="Error reading spec file %s: unexpected EOF\n",
_tag_dyneither(_tmp3B0,sizeof(char),44))),_tag_dyneither(_tmp3AF,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);goto CLEANUP_AND_RETURN;}if(c == ':'){
strname[_check_known_subscript_notnull(256,i)]='\000';break;}strname[
_check_known_subscript_notnull(256,i)]=(char)c;++ i;if(i >= 256){{const char*
_tmp3B4;void*_tmp3B3[1];struct Cyc_String_pa_struct _tmp3B2;(_tmp3B2.tag=0,((
_tmp3B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp3B3[0]=&
_tmp3B2,Cyc_fprintf(Cyc_stderr,((_tmp3B4="Error reading spec file %s: string name too long\n",
_tag_dyneither(_tmp3B4,sizeof(char),50))),_tag_dyneither(_tmp3B3,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);goto CLEANUP_AND_RETURN;}}while(1){c=
Cyc_fgetc((struct Cyc___cycFILE*)spec_file);if(isspace(c))continue;break;}if(c == '*'){{
struct _tuple19*_tmp3C1;const char*_tmp3C0;struct _dyneither_ptr*_tmp3BF;struct
_dyneither_ptr*_tmp3BE;struct Cyc_List_List*_tmp3BD;_tmp3A=((_tmp3BD=_cycalloc(
sizeof(*_tmp3BD)),((_tmp3BD->hd=((_tmp3C1=_cycalloc(sizeof(*_tmp3C1)),((_tmp3C1->f1=((
_tmp3BE=_cycalloc(sizeof(*_tmp3BE)),((_tmp3BE[0]=(struct _dyneither_ptr)Cyc_strdup(
_tag_dyneither(strname,sizeof(char),256)),_tmp3BE)))),((_tmp3C1->f2=((_tmp3BF=
_cycalloc(sizeof(*_tmp3BF)),((_tmp3BF[0]=(struct _dyneither_ptr)Cyc_strdup(((
_tmp3C0="",_tag_dyneither(_tmp3C0,sizeof(char),1)))),_tmp3BF)))),_tmp3C1)))))),((
_tmp3BD->tl=_tmp3A,_tmp3BD))))));}goto JUST_AFTER_STAR;}strvalue[0]=(char)c;i=1;
while(1){c=Cyc_fgetc((struct Cyc___cycFILE*)spec_file);if((c == - 1  || c == '\n')
 || c == '\r'){strvalue[_check_known_subscript_notnull(4096,i)]='\000';break;}
strvalue[_check_known_subscript_notnull(4096,i)]=(char)c;++ i;if(i >= 4096){{const
char*_tmp3C6;void*_tmp3C5[2];struct Cyc_String_pa_struct _tmp3C4;struct Cyc_String_pa_struct
_tmp3C3;(_tmp3C3.tag=0,((_tmp3C3.f1=(struct _dyneither_ptr)_tag_dyneither(strname,
sizeof(char),256),((_tmp3C4.tag=0,((_tmp3C4.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)file),((_tmp3C5[0]=& _tmp3C4,((_tmp3C5[1]=& _tmp3C3,Cyc_fprintf(Cyc_stderr,((
_tmp3C6="Error reading spec file %s: value of %s too long\n",_tag_dyneither(
_tmp3C6,sizeof(char),50))),_tag_dyneither(_tmp3C5,sizeof(void*),2)))))))))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);goto CLEANUP_AND_RETURN;}}{struct
_tuple19*_tmp3CF;struct _dyneither_ptr*_tmp3CE;struct _dyneither_ptr*_tmp3CD;
struct Cyc_List_List*_tmp3CC;_tmp3A=((_tmp3CC=_cycalloc(sizeof(*_tmp3CC)),((
_tmp3CC->hd=((_tmp3CF=_cycalloc(sizeof(*_tmp3CF)),((_tmp3CF->f1=((_tmp3CD=
_cycalloc(sizeof(*_tmp3CD)),((_tmp3CD[0]=(struct _dyneither_ptr)Cyc_strdup(
_tag_dyneither(strname,sizeof(char),256)),_tmp3CD)))),((_tmp3CF->f2=((_tmp3CE=
_cycalloc(sizeof(*_tmp3CE)),((_tmp3CE[0]=(struct _dyneither_ptr)Cyc_strdup(
_tag_dyneither(strvalue,sizeof(char),4096)),_tmp3CE)))),_tmp3CF)))))),((_tmp3CC->tl=
_tmp3A,_tmp3CC))))));}if(c == - 1)goto CLEANUP_AND_RETURN;}CLEANUP_AND_RETURN: Cyc_fclose((
struct Cyc___cycFILE*)spec_file);return _tmp3A;}struct _dyneither_ptr Cyc_split_specs(
struct _dyneither_ptr cmdline);static void _tmp3D9(struct _dyneither_ptr*_tmp5B,
unsigned int*_tmp3D8,unsigned int*_tmp3D7,struct _dyneither_ptr**_tmp3D5){for(*
_tmp3D8=0;*_tmp3D8 < *_tmp3D7;(*_tmp3D8)++){(*_tmp3D5)[*_tmp3D8]=*((struct
_dyneither_ptr**)(*_tmp5B).curr)[(int)*_tmp3D8];}}struct _dyneither_ptr Cyc_split_specs(
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
4096,j)]='\000';{struct _dyneither_ptr*_tmp3D2;struct Cyc_List_List*_tmp3D1;_tmp56=((
_tmp3D1=_cycalloc(sizeof(*_tmp3D1)),((_tmp3D1->hd=((_tmp3D2=_cycalloc(sizeof(*
_tmp3D2)),((_tmp3D2[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(buf,
sizeof(char),4096)),_tmp3D2)))),((_tmp3D1->tl=_tmp56,_tmp3D1))))));};}DONE:
_tmp56=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp56);{
struct Cyc_List_List*_tmp3D3;_tmp56=((_tmp3D3=_cycalloc(sizeof(*_tmp3D3)),((
_tmp3D3->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",
sizeof(char),1),((_tmp3D3->tl=_tmp56,_tmp3D3))))));}{struct _dyneither_ptr _tmp5B=((
struct _dyneither_ptr(*)(struct Cyc_List_List*x))Cyc_List_to_array)(_tmp56);
unsigned int _tmp3D8;unsigned int _tmp3D7;struct _dyneither_ptr _tmp3D6;struct
_dyneither_ptr*_tmp3D5;unsigned int _tmp3D4;struct _dyneither_ptr _tmp5C=(_tmp3D4=
_get_dyneither_size(_tmp5B,sizeof(struct _dyneither_ptr*)),((_tmp3D5=(struct
_dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp3D4)),((
_tmp3D6=_tag_dyneither(_tmp3D5,sizeof(struct _dyneither_ptr),_tmp3D4),((((_tmp3D7=
_tmp3D4,_tmp3D9(& _tmp5B,& _tmp3D8,& _tmp3D7,& _tmp3D5))),_tmp3D6)))))));return
_tmp5C;};};}int Cyc_compile_failure=0;struct Cyc___cycFILE*Cyc_try_file_open(
struct _dyneither_ptr filename,struct _dyneither_ptr mode,struct _dyneither_ptr
msg_part);struct Cyc___cycFILE*Cyc_try_file_open(struct _dyneither_ptr filename,
struct _dyneither_ptr mode,struct _dyneither_ptr msg_part){struct _handler_cons _tmp61;
_push_handler(& _tmp61);{int _tmp63=0;if(setjmp(_tmp61.handler))_tmp63=1;if(!
_tmp63){{struct Cyc___cycFILE*_tmp64=(struct Cyc___cycFILE*)Cyc_file_open(filename,
mode);_npop_handler(0);return _tmp64;};_pop_handler();}else{void*_tmp62=(void*)
_exn_thrown;void*_tmp66=_tmp62;_LL1:;_LL2: Cyc_compile_failure=1;{const char*
_tmp3DE;void*_tmp3DD[2];struct Cyc_String_pa_struct _tmp3DC;struct Cyc_String_pa_struct
_tmp3DB;(_tmp3DB.tag=0,((_tmp3DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
filename),((_tmp3DC.tag=0,((_tmp3DC.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)msg_part),((_tmp3DD[0]=& _tmp3DC,((_tmp3DD[1]=& _tmp3DB,Cyc_fprintf(
Cyc_stderr,((_tmp3DE="\nError: couldn't open %s %s\n",_tag_dyneither(_tmp3DE,
sizeof(char),29))),_tag_dyneither(_tmp3DD,sizeof(void*),2)))))))))))));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);return 0;_LL3:;_LL4:(void)_throw(_tmp66);_LL0:;}};}
struct Cyc_List_List*Cyc_do_stage(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*
f)(void*,struct Cyc_List_List*),void*env,struct Cyc_List_List*tds);struct Cyc_List_List*
Cyc_do_stage(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(void*,
struct Cyc_List_List*),void*env,struct Cyc_List_List*tds){const char*_tmp3DF;struct
_dyneither_ptr exn_string=(_tmp3DF="",_tag_dyneither(_tmp3DF,sizeof(char),1));
const char*_tmp3E0;struct _dyneither_ptr explain_string=(_tmp3E0="",_tag_dyneither(
_tmp3E0,sizeof(char),1));int other_exn=0;struct Cyc_Core_Impossible_struct _tmp3E6;
const char*_tmp3E5;struct Cyc_Core_Impossible_struct*_tmp3E4;void*ex=(void*)((
_tmp3E4=_cycalloc(sizeof(*_tmp3E4)),((_tmp3E4[0]=((_tmp3E6.tag=Cyc_Core_Impossible,((
_tmp3E6.f1=((_tmp3E5="",_tag_dyneither(_tmp3E5,sizeof(char),1))),_tmp3E6)))),
_tmp3E4))));struct Cyc_List_List*_tmp6B=0;{struct _handler_cons _tmp6C;
_push_handler(& _tmp6C);{int _tmp6E=0;if(setjmp(_tmp6C.handler))_tmp6E=1;if(!
_tmp6E){_tmp6B=f(env,tds);;_pop_handler();}else{void*_tmp6D=(void*)_exn_thrown;
void*_tmp70=_tmp6D;struct _dyneither_ptr _tmp72;struct _dyneither_ptr _tmp75;struct
_dyneither_ptr _tmp77;_LL6: {struct Cyc_Core_Impossible_struct*_tmp71=(struct Cyc_Core_Impossible_struct*)
_tmp70;if(_tmp71->tag != Cyc_Core_Impossible)goto _LL8;else{_tmp72=_tmp71->f1;}}
_LL7:{const char*_tmp3E7;exn_string=((_tmp3E7="Exception Core::Impossible",
_tag_dyneither(_tmp3E7,sizeof(char),27)));}explain_string=_tmp72;goto _LL5;_LL8: {
struct Cyc_Dict_Absent_struct*_tmp73=(struct Cyc_Dict_Absent_struct*)_tmp70;if(
_tmp73->tag != Cyc_Dict_Absent)goto _LLA;}_LL9:{const char*_tmp3E8;exn_string=((
_tmp3E8="Exception Dict::Absent",_tag_dyneither(_tmp3E8,sizeof(char),23)));}goto
_LL5;_LLA: {struct Cyc_Core_Invalid_argument_struct*_tmp74=(struct Cyc_Core_Invalid_argument_struct*)
_tmp70;if(_tmp74->tag != Cyc_Core_Invalid_argument)goto _LLC;else{_tmp75=_tmp74->f1;}}
_LLB:{const char*_tmp3E9;exn_string=((_tmp3E9="Exception Core::Invalid_argument",
_tag_dyneither(_tmp3E9,sizeof(char),33)));}explain_string=_tmp75;goto _LL5;_LLC: {
struct Cyc_Core_Failure_struct*_tmp76=(struct Cyc_Core_Failure_struct*)_tmp70;if(
_tmp76->tag != Cyc_Core_Failure)goto _LLE;else{_tmp77=_tmp76->f1;}}_LLD:{const char*
_tmp3EA;exn_string=((_tmp3EA="Exception Core::Failure",_tag_dyneither(_tmp3EA,
sizeof(char),24)));}explain_string=_tmp77;goto _LL5;_LLE:;_LLF: ex=_tmp70;
other_exn=1;{const char*_tmp3EB;exn_string=((_tmp3EB="Uncaught exception",
_tag_dyneither(_tmp3EB,sizeof(char),19)));}goto _LL5;_LL10:;_LL11:(void)_throw(
_tmp70);_LL5:;}};}if(Cyc_Position_error_p())Cyc_compile_failure=1;{const char*
_tmp3EC;if(Cyc_strcmp((struct _dyneither_ptr)exn_string,((_tmp3EC="",
_tag_dyneither(_tmp3EC,sizeof(char),1))))!= 0){Cyc_compile_failure=1;{const char*
_tmp3F2;void*_tmp3F1[3];struct Cyc_String_pa_struct _tmp3F0;struct Cyc_String_pa_struct
_tmp3EF;struct Cyc_String_pa_struct _tmp3EE;(_tmp3EE.tag=0,((_tmp3EE.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)explain_string),((_tmp3EF.tag=0,((_tmp3EF.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)stage_name),((_tmp3F0.tag=0,((
_tmp3F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)exn_string),((_tmp3F1[0]=&
_tmp3F0,((_tmp3F1[1]=& _tmp3EF,((_tmp3F1[2]=& _tmp3EE,Cyc_fprintf(Cyc_stderr,((
_tmp3F2="\n%s thrown during %s: %s",_tag_dyneither(_tmp3F2,sizeof(char),25))),
_tag_dyneither(_tmp3F1,sizeof(void*),3)))))))))))))))))));};}}if(Cyc_compile_failure){{
const char*_tmp3F5;void*_tmp3F4;(_tmp3F4=0,Cyc_fprintf(Cyc_stderr,((_tmp3F5="\nCOMPILATION FAILED!\n",
_tag_dyneither(_tmp3F5,sizeof(char),22))),_tag_dyneither(_tmp3F4,sizeof(void*),0)));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);if(other_exn)(int)_throw(ex);return
_tmp6B;}else{if(Cyc_v_r){{const char*_tmp3F9;void*_tmp3F8[1];struct Cyc_String_pa_struct
_tmp3F7;(_tmp3F7.tag=0,((_tmp3F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
stage_name),((_tmp3F8[0]=& _tmp3F7,Cyc_fprintf(Cyc_stderr,((_tmp3F9="%s completed.\n",
_tag_dyneither(_tmp3F9,sizeof(char),15))),_tag_dyneither(_tmp3F8,sizeof(void*),1)))))));}
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
_tmp94);const char*_tmp3FE;const char*_tmp3FD;struct _tuple18*_tmp3FC;if(!Cyc_Interface_is_subinterface(
_tmp97,_tmp96,((_tmp3FC=_cycalloc(sizeof(*_tmp3FC)),((_tmp3FC->f1=((_tmp3FE="written interface",
_tag_dyneither(_tmp3FE,sizeof(char),18))),((_tmp3FC->f2=((_tmp3FD="maximal interface",
_tag_dyneither(_tmp3FD,sizeof(char),18))),_tmp3FC))))))))Cyc_compile_failure=1;
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
out_file){if(file.curr == (_tag_dyneither(0,0,0)).curr){{const char*_tmp401;void*
_tmp400;(_tmp400=0,Cyc_fprintf(Cyc_stderr,((_tmp401="Internal error: copy_internal_file called with NULL\n",
_tag_dyneither(_tmp401,sizeof(char),53))),_tag_dyneither(_tmp400,sizeof(void*),0)));}
return 1;}{const char*_tmp403;const char*_tmp402;struct Cyc___cycFILE*_tmp9D=Cyc_try_file_open(
file,((_tmp402="r",_tag_dyneither(_tmp402,sizeof(char),2))),((_tmp403="internal compiler file",
_tag_dyneither(_tmp403,sizeof(char),23))));if(_tmp9D == 0)return 1;{unsigned long
n_read=1024;unsigned long n_written;char buf[1024];{unsigned int _tmpA4=1024;
unsigned int i;for(i=0;i < _tmpA4;i ++){buf[i]='\000';}}while(n_read == 1024){n_read=
Cyc_fread(_tag_dyneither(buf,sizeof(char),1024),1,1024,(struct Cyc___cycFILE*)
_tmp9D);if(n_read != 1024  && !Cyc_feof((struct Cyc___cycFILE*)_tmp9D)){Cyc_compile_failure=
1;{const char*_tmp407;void*_tmp406[1];struct Cyc_String_pa_struct _tmp405;(_tmp405.tag=
0,((_tmp405.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp406[0]=&
_tmp405,Cyc_fprintf(Cyc_stderr,((_tmp407="\nError: problem copying %s\n",
_tag_dyneither(_tmp407,sizeof(char),28))),_tag_dyneither(_tmp406,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 1;}n_written=Cyc_fwrite(
_tag_dyneither(buf,sizeof(char),1024),1,n_read,out_file);if(n_read != n_written){
Cyc_compile_failure=1;{const char*_tmp40B;void*_tmp40A[1];struct Cyc_String_pa_struct
_tmp409;(_tmp409.tag=0,((_tmp409.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
file),((_tmp40A[0]=& _tmp409,Cyc_fprintf(Cyc_stderr,((_tmp40B="\nError: problem copying %s\n",
_tag_dyneither(_tmp40B,sizeof(char),28))),_tag_dyneither(_tmp40A,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 1;}}Cyc_fclose((struct Cyc___cycFILE*)
_tmp9D);return 0;};};}struct Cyc_List_List*Cyc_do_print(struct Cyc___cycFILE*
out_file,struct Cyc_List_List*tds);struct Cyc_List_List*Cyc_do_print(struct Cyc___cycFILE*
out_file,struct Cyc_List_List*tds){struct Cyc_Absynpp_Params _tmpA7=Cyc_tc_r?Cyc_Absynpp_cyc_params_r:
Cyc_Absynpp_c_params_r;_tmpA7.expand_typedefs=!Cyc_noexpand_r;_tmpA7.to_VC=Cyc_Cyclone_tovc_r;
_tmpA7.add_cyc_prefix=Cyc_add_cyc_namespace_r;_tmpA7.generate_line_directives=
Cyc_generate_line_directives_r;_tmpA7.print_full_evars=Cyc_print_full_evars_r;
_tmpA7.print_all_tvars=Cyc_print_all_tvars_r;_tmpA7.print_all_kinds=Cyc_print_all_kinds_r;
_tmpA7.print_all_effects=Cyc_print_all_effects_r;if(Cyc_inline_functions_r){
const char*_tmp40E;void*_tmp40D;(_tmp40D=0,Cyc_fprintf(out_file,((_tmp40E="#define _INLINE_FUNCTIONS\n",
_tag_dyneither(_tmp40E,sizeof(char),27))),_tag_dyneither(_tmp40D,sizeof(void*),0)));}
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
_tag_dyneither(0,0,0)).curr){{struct _dyneither_ptr*_tmp411;struct Cyc_List_List*
_tmp410;_tmpAA=((_tmp410=_cycalloc(sizeof(*_tmp410)),((_tmp410->hd=((_tmp411=
_cycalloc(sizeof(*_tmp411)),((_tmp411[0]=s,_tmp411)))),((_tmp410->tl=_tmpAA,
_tmp410))))));}break;}else{{struct _dyneither_ptr*_tmp414;struct Cyc_List_List*
_tmp413;_tmpAA=((_tmp413=_cycalloc(sizeof(*_tmp413)),((_tmp413->hd=((_tmp414=
_cycalloc(sizeof(*_tmp414)),((_tmp414[0]=(struct _dyneither_ptr)Cyc_substring((
struct _dyneither_ptr)s,0,(unsigned long)((((struct _dyneither_ptr)_tmpAC).curr - s.curr)
/ sizeof(char))),_tmp414)))),((_tmp413->tl=_tmpAA,_tmp413))))));}_tmpAB -=(((
struct _dyneither_ptr)_tmpAC).curr - s.curr)/ sizeof(char);s=_dyneither_ptr_plus(
_tmpAC,sizeof(char),1);}}return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmpAA);};}static int Cyc_file_exists(struct _dyneither_ptr file);
static int Cyc_file_exists(struct _dyneither_ptr file){struct Cyc___cycFILE*f=0;{
struct _handler_cons _tmpB1;_push_handler(& _tmpB1);{int _tmpB3=0;if(setjmp(_tmpB1.handler))
_tmpB3=1;if(!_tmpB3){{const char*_tmp415;f=(struct Cyc___cycFILE*)Cyc_file_open(
file,((_tmp415="r",_tag_dyneither(_tmp415,sizeof(char),2))));};_pop_handler();}
else{void*_tmpB2=(void*)_exn_thrown;void*_tmpB6=_tmpB2;_LL13:;_LL14: goto _LL12;
_LL15:;_LL16:(void)_throw(_tmpB6);_LL12:;}};}if(f == 0)return 0;else{Cyc_fclose((
struct Cyc___cycFILE*)f);return 1;}}static struct _dyneither_ptr Cyc_sprint_list(
struct Cyc_List_List*dirs);static struct _dyneither_ptr Cyc_sprint_list(struct Cyc_List_List*
dirs){const char*_tmp416;struct _dyneither_ptr tmp=(_tmp416="",_tag_dyneither(
_tmp416,sizeof(char),1));for(0;dirs != 0;dirs=dirs->tl){struct _dyneither_ptr
_tmpB7=*((struct _dyneither_ptr*)dirs->hd);if(_tmpB7.curr == (_tag_dyneither(0,0,0)).curr
 || Cyc_strlen((struct _dyneither_ptr)_tmpB7)== 0)continue;{const char*_tmp417;
_tmpB7=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmpB7,((
_tmp417=":",_tag_dyneither(_tmp417,sizeof(char),2))));}tmp=(struct _dyneither_ptr)
Cyc_strconcat((struct _dyneither_ptr)_tmpB7,(struct _dyneither_ptr)tmp);}return tmp;}
static struct _dyneither_ptr*Cyc_find(struct Cyc_List_List*dirs,struct
_dyneither_ptr file);static struct _dyneither_ptr*Cyc_find(struct Cyc_List_List*dirs,
struct _dyneither_ptr file){if(file.curr == (_tag_dyneither(0,0,0)).curr)return 0;
for(0;dirs != 0;dirs=dirs->tl){struct _dyneither_ptr _tmpBA=*((struct _dyneither_ptr*)
dirs->hd);if(_tmpBA.curr == (_tag_dyneither(0,0,0)).curr  || Cyc_strlen((struct
_dyneither_ptr)_tmpBA)== 0)continue;{struct _dyneither_ptr s=(struct _dyneither_ptr)
Cyc_Filename_concat(_tmpBA,file);if(Cyc_file_exists(s)){struct _dyneither_ptr*
_tmp418;return(_tmp418=_cycalloc(sizeof(*_tmp418)),((_tmp418[0]=s,_tmp418)));}};}
return 0;}static struct _dyneither_ptr Cyc_do_find(struct Cyc_List_List*dirs,struct
_dyneither_ptr file);static struct _dyneither_ptr Cyc_do_find(struct Cyc_List_List*
dirs,struct _dyneither_ptr file){struct _dyneither_ptr*_tmpBC=Cyc_find(dirs,file);
if(_tmpBC == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();{const char*_tmp41D;void*
_tmp41C[2];struct Cyc_String_pa_struct _tmp41B;struct Cyc_String_pa_struct _tmp41A;(
_tmp41A.tag=0,((_tmp41A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sprint_list(
dirs)),((_tmp41B.tag=0,((_tmp41B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
file),((_tmp41C[0]=& _tmp41B,((_tmp41C[1]=& _tmp41A,Cyc_fprintf(Cyc_stderr,((
_tmp41D="Error: can't find internal compiler file %s in path %s\n",
_tag_dyneither(_tmp41D,sizeof(char),56))),_tag_dyneither(_tmp41C,sizeof(void*),2)))))))))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Core_Failure_struct
_tmp42D;const char*_tmp42C;void*_tmp42B[2];struct Cyc_String_pa_struct _tmp42A;
struct Cyc_String_pa_struct _tmp429;struct Cyc_Core_Failure_struct*_tmp428;(int)
_throw((void*)((_tmp428=_cycalloc(sizeof(*_tmp428)),((_tmp428[0]=((_tmp42D.tag=
Cyc_Core_Failure,((_tmp42D.f1=(struct _dyneither_ptr)((_tmp429.tag=0,((_tmp429.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sprint_list(dirs)),((_tmp42A.tag=
0,((_tmp42A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp42B[0]=&
_tmp42A,((_tmp42B[1]=& _tmp429,Cyc_aprintf(((_tmp42C="Error: can't find internal compiler file %s in path %s\n",
_tag_dyneither(_tmp42C,sizeof(char),56))),_tag_dyneither(_tmp42B,sizeof(void*),2)))))))))))))),
_tmp42D)))),_tmp428)))));};}return*_tmpBC;}static struct Cyc_List_List*Cyc_add_subdir(
struct Cyc_List_List*dirs,struct _dyneither_ptr subdir);static struct Cyc_List_List*
Cyc_add_subdir(struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){struct Cyc_List_List*
_tmpC7=0;for(0;dirs != 0;dirs=dirs->tl){struct _dyneither_ptr*_tmp430;struct Cyc_List_List*
_tmp42F;_tmpC7=((_tmp42F=_cycalloc(sizeof(*_tmp42F)),((_tmp42F->hd=((_tmp430=
_cycalloc(sizeof(*_tmp430)),((_tmp430[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((
struct _dyneither_ptr*)dirs->hd),subdir),_tmp430)))),((_tmp42F->tl=_tmpC7,_tmp42F))))));}
_tmpC7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpC7);
return _tmpC7;}static struct Cyc_List_List*Cyc_also_subdir(struct Cyc_List_List*dirs,
struct _dyneither_ptr subdir);static struct Cyc_List_List*Cyc_also_subdir(struct Cyc_List_List*
dirs,struct _dyneither_ptr subdir){struct Cyc_List_List*_tmpCA=0;for(0;dirs != 0;
dirs=dirs->tl){{struct Cyc_List_List*_tmp431;_tmpCA=((_tmp431=_cycalloc(sizeof(*
_tmp431)),((_tmp431->hd=(struct _dyneither_ptr*)dirs->hd,((_tmp431->tl=_tmpCA,
_tmp431))))));}{struct _dyneither_ptr*_tmp434;struct Cyc_List_List*_tmp433;_tmpCA=((
_tmp433=_cycalloc(sizeof(*_tmp433)),((_tmp433->hd=((_tmp434=_cycalloc(sizeof(*
_tmp434)),((_tmp434[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct
_dyneither_ptr*)dirs->hd),subdir),_tmp434)))),((_tmp433->tl=_tmpCA,_tmp433))))));};}
_tmpCA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpCA);
return _tmpCA;}static int Cyc_is_other_special(char c);static int Cyc_is_other_special(
char c){switch(c){case '\\': _LL17: goto _LL18;case '"': _LL18: goto _LL19;case ';': _LL19:
goto _LL1A;case '&': _LL1A: goto _LL1B;case '(': _LL1B: goto _LL1C;case ')': _LL1C: goto
_LL1D;case '|': _LL1D: goto _LL1E;case '^': _LL1E: goto _LL1F;case '<': _LL1F: goto _LL20;
case '>': _LL20: goto _LL21;case ' ': _LL21: goto _LL22;case '\n': _LL22: goto _LL23;case '\t':
_LL23: return 1;default: _LL24: return 0;}}static struct _dyneither_ptr Cyc_sh_escape_string(
struct _dyneither_ptr s);static void _tmp43D(unsigned int*_tmp43C,unsigned int*
_tmp43B,char**_tmp439){for(*_tmp43C=0;*_tmp43C < *_tmp43B;(*_tmp43C)++){(*_tmp439)[*
_tmp43C]='\000';}}static struct _dyneither_ptr Cyc_sh_escape_string(struct
_dyneither_ptr s){unsigned long _tmpCE=Cyc_strlen((struct _dyneither_ptr)s);int
_tmpCF=0;int _tmpD0=0;{int i=0;for(0;i < _tmpCE;++ i){char _tmpD1=*((const char*)
_check_dyneither_subscript(s,sizeof(char),i));if(_tmpD1 == '\'')++ _tmpCF;else{if(
Cyc_is_other_special(_tmpD1))++ _tmpD0;}}}if(_tmpCF == 0  && _tmpD0 == 0)return s;if(
_tmpCF == 0){struct _dyneither_ptr*_tmp437;struct _dyneither_ptr*_tmp436[3];return(
struct _dyneither_ptr)Cyc_strconcat_l(((_tmp436[2]=_init_dyneither_ptr(_cycalloc(
sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((_tmp436[1]=((_tmp437=
_cycalloc(sizeof(*_tmp437)),((_tmp437[0]=(struct _dyneither_ptr)s,_tmp437)))),((
_tmp436[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",
sizeof(char),2),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp436,sizeof(struct _dyneither_ptr*),3)))))))));}{unsigned long
_tmpD6=(_tmpCE + _tmpCF)+ _tmpD0;unsigned int _tmp43C;unsigned int _tmp43B;struct
_dyneither_ptr _tmp43A;char*_tmp439;unsigned int _tmp438;struct _dyneither_ptr s2=(
_tmp438=_tmpD6 + 1,((_tmp439=(char*)_cycalloc_atomic(_check_times(sizeof(char),
_tmp438 + 1)),((_tmp43A=_tag_dyneither(_tmp439,sizeof(char),_tmp438 + 1),((((
_tmp43B=_tmp438,((_tmp43D(& _tmp43C,& _tmp43B,& _tmp439),_tmp439[_tmp43B]=(char)0)))),
_tmp43A)))))));int _tmpD7=0;int _tmpD8=0;for(0;_tmpD7 < _tmpCE;++ _tmpD7){char _tmpD9=*((
const char*)_check_dyneither_subscript(s,sizeof(char),_tmpD7));if(_tmpD9 == '\''
 || Cyc_is_other_special(_tmpD9)){char _tmp440;char _tmp43F;struct _dyneither_ptr
_tmp43E;(_tmp43E=_dyneither_ptr_plus(s2,sizeof(char),_tmpD8 ++),((_tmp43F=*((char*)
_check_dyneither_subscript(_tmp43E,sizeof(char),0)),((_tmp440='\\',((
_get_dyneither_size(_tmp43E,sizeof(char))== 1  && (_tmp43F == '\000'  && _tmp440 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp43E.curr)=_tmp440)))))));}{char _tmp443;char
_tmp442;struct _dyneither_ptr _tmp441;(_tmp441=_dyneither_ptr_plus(s2,sizeof(char),
_tmpD8 ++),((_tmp442=*((char*)_check_dyneither_subscript(_tmp441,sizeof(char),0)),((
_tmp443=_tmpD9,((_get_dyneither_size(_tmp441,sizeof(char))== 1  && (_tmp442 == '\000'
 && _tmp443 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp441.curr)=_tmp443)))))));};}
return(struct _dyneither_ptr)s2;};}static struct _dyneither_ptr*Cyc_sh_escape_stringptr(
struct _dyneither_ptr*sp);static struct _dyneither_ptr*Cyc_sh_escape_stringptr(
struct _dyneither_ptr*sp){struct _dyneither_ptr*_tmp444;return(_tmp444=_cycalloc(
sizeof(*_tmp444)),((_tmp444[0]=Cyc_sh_escape_string(*sp),_tmp444)));}static void
Cyc_process_file(struct _dyneither_ptr filename);static void Cyc_process_file(struct
_dyneither_ptr filename){struct _dyneither_ptr _tmpE5=Cyc_make_base_filename(
filename,Cyc_output_file);const char*_tmp445;struct _dyneither_ptr _tmpE6=Cyc_strconcat((
struct _dyneither_ptr)_tmpE5,((_tmp445=".cyp",_tag_dyneither(_tmp445,sizeof(char),
5))));const char*_tmp446;struct _dyneither_ptr _tmpE7=Cyc_strconcat((struct
_dyneither_ptr)_tmpE5,((_tmp446=".cyci",_tag_dyneither(_tmp446,sizeof(char),6))));
const char*_tmp447;struct _dyneither_ptr _tmpE8=Cyc_strconcat((struct _dyneither_ptr)
_tmpE5,((_tmp447=".cycio",_tag_dyneither(_tmp447,sizeof(char),7))));const char*
_tmp448;struct _dyneither_ptr _tmpE9=Cyc_strconcat((struct _dyneither_ptr)_tmpE5,((
_tmp448=".c",_tag_dyneither(_tmp448,sizeof(char),3))));if(Cyc_v_r){const char*
_tmp44C;void*_tmp44B[1];struct Cyc_String_pa_struct _tmp44A;(_tmp44A.tag=0,((
_tmp44A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename),((_tmp44B[0]=&
_tmp44A,Cyc_fprintf(Cyc_stderr,((_tmp44C="Compiling %s\n",_tag_dyneither(_tmp44C,
sizeof(char),14))),_tag_dyneither(_tmp44B,sizeof(void*),1)))))));}{const char*
_tmp44E;const char*_tmp44D;struct Cyc___cycFILE*f0=Cyc_try_file_open(filename,((
_tmp44D="r",_tag_dyneither(_tmp44D,sizeof(char),2))),((_tmp44E="input file",
_tag_dyneither(_tmp44E,sizeof(char),11))));if(Cyc_compile_failure  || !((
unsigned int)f0))return;Cyc_fclose((struct Cyc___cycFILE*)f0);{const char*_tmp455;
struct _dyneither_ptr*_tmp454;const char*_tmp453;struct Cyc_List_List*_tmp452;
struct _dyneither_ptr _tmpED=Cyc_str_sepstr(((_tmp452=_cycalloc(sizeof(*_tmp452)),((
_tmp452->hd=((_tmp454=_cycalloc(sizeof(*_tmp454)),((_tmp454[0]=(struct
_dyneither_ptr)((_tmp453="",_tag_dyneither(_tmp453,sizeof(char),1))),_tmp454)))),((
_tmp452->tl=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs)),
_tmp452)))))),((_tmp455=" ",_tag_dyneither(_tmp455,sizeof(char),2))));struct
_dyneither_ptr def_inc_path=(struct _dyneither_ptr)Cstring_to_string(Cdef_inc_path);
struct Cyc_List_List*_tmpEE=Cyc_add_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);{
const char*_tmp456;_tmpEE=Cyc_add_subdir(_tmpEE,((_tmp456="include",
_tag_dyneither(_tmp456,sizeof(char),8))));}if(Cyc_strlen((struct _dyneither_ptr)
def_inc_path)> 0){struct _dyneither_ptr*_tmp459;struct Cyc_List_List*_tmp458;
_tmpEE=((_tmp458=_cycalloc(sizeof(*_tmp458)),((_tmp458->hd=((_tmp459=_cycalloc(
sizeof(*_tmp459)),((_tmp459[0]=def_inc_path,_tmp459)))),((_tmp458->tl=_tmpEE,
_tmp458))))));}{char*_tmpF2=getenv((const char*)"CYCLONE_INCLUDE_PATH");if((char*)
_tmpF2 != 0){char*_tmp45A;_tmpEE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(Cyc_split_by_char(((_tmp45A=_tmpF2,
_tag_dyneither(_tmp45A,sizeof(char),_get_zero_arr_size_char(_tmp45A,1)))),':'),
_tmpEE);}{const char*_tmp461;struct _dyneither_ptr*_tmp460;const char*_tmp45F;
struct Cyc_List_List*_tmp45E;struct _dyneither_ptr stdinc_string=(struct
_dyneither_ptr)Cyc_str_sepstr(((_tmp45E=_cycalloc(sizeof(*_tmp45E)),((_tmp45E->hd=((
_tmp460=_cycalloc(sizeof(*_tmp460)),((_tmp460[0]=(struct _dyneither_ptr)((_tmp45F="",
_tag_dyneither(_tmp45F,sizeof(char),1))),_tmp460)))),((_tmp45E->tl=((struct Cyc_List_List*(*)(
struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,_tmpEE),_tmp45E)))))),((_tmp461=" -I",_tag_dyneither(
_tmp461,sizeof(char),4))));struct _dyneither_ptr ofile_string;if(Cyc_stop_after_cpp_r){
if(Cyc_output_file != 0){const char*_tmp465;void*_tmp464[1];struct Cyc_String_pa_struct
_tmp463;ofile_string=(struct _dyneither_ptr)((_tmp463.tag=0,((_tmp463.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_output_file))),((
_tmp464[0]=& _tmp463,Cyc_aprintf(((_tmp465=" > %s",_tag_dyneither(_tmp465,sizeof(
char),6))),_tag_dyneither(_tmp464,sizeof(void*),1))))))));}else{const char*
_tmp466;ofile_string=((_tmp466="",_tag_dyneither(_tmp466,sizeof(char),1)));}}
else{const char*_tmp46A;void*_tmp469[1];struct Cyc_String_pa_struct _tmp468;
ofile_string=(struct _dyneither_ptr)((_tmp468.tag=0,((_tmp468.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string((struct _dyneither_ptr)
_tmpE6)),((_tmp469[0]=& _tmp468,Cyc_aprintf(((_tmp46A=" > %s",_tag_dyneither(
_tmp46A,sizeof(char),6))),_tag_dyneither(_tmp469,sizeof(void*),1))))))));}{
struct _dyneither_ptr fixup_string;if(Cyc_produce_dependencies){if(Cyc_dependencies_target
== 0){const char*_tmp46B;fixup_string=((_tmp46B=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'",
_tag_dyneither(_tmp46B,sizeof(char),35)));}else{const char*_tmp46F;void*_tmp46E[1];
struct Cyc_String_pa_struct _tmp46D;fixup_string=(struct _dyneither_ptr)((_tmp46D.tag=
0,((_tmp46D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct
_dyneither_ptr*)_check_null(Cyc_dependencies_target))),((_tmp46E[0]=& _tmp46D,Cyc_aprintf(((
_tmp46F=" | sed 's/^.*\\.cyc\\.o:/%s:/'",_tag_dyneither(_tmp46F,sizeof(char),29))),
_tag_dyneither(_tmp46E,sizeof(void*),1))))))));}}else{const char*_tmp470;
fixup_string=((_tmp470="",_tag_dyneither(_tmp470,sizeof(char),1)));}{const char*
_tmp479;void*_tmp478[6];struct Cyc_String_pa_struct _tmp477;struct Cyc_String_pa_struct
_tmp476;struct Cyc_String_pa_struct _tmp475;struct Cyc_String_pa_struct _tmp474;
struct Cyc_String_pa_struct _tmp473;struct Cyc_String_pa_struct _tmp472;struct
_dyneither_ptr _tmp100=(_tmp472.tag=0,((_tmp472.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)ofile_string),((_tmp473.tag=0,((_tmp473.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)fixup_string),((_tmp474.tag=0,((_tmp474.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(filename)),((_tmp475.tag=
0,((_tmp475.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdinc_string),((
_tmp476.tag=0,((_tmp476.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpED),((
_tmp477.tag=0,((_tmp477.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cpp),((
_tmp478[0]=& _tmp477,((_tmp478[1]=& _tmp476,((_tmp478[2]=& _tmp475,((_tmp478[3]=&
_tmp474,((_tmp478[4]=& _tmp473,((_tmp478[5]=& _tmp472,Cyc_aprintf(((_tmp479="%s %s%s %s%s%s",
_tag_dyneither(_tmp479,sizeof(char),15))),_tag_dyneither(_tmp478,sizeof(void*),6)))))))))))))))))))))))))))))))))))));
if(Cyc_v_r){const char*_tmp47D;void*_tmp47C[1];struct Cyc_String_pa_struct _tmp47B;(
_tmp47B.tag=0,((_tmp47B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp100),((
_tmp47C[0]=& _tmp47B,Cyc_fprintf(Cyc_stderr,((_tmp47D="%s\n",_tag_dyneither(
_tmp47D,sizeof(char),4))),_tag_dyneither(_tmp47C,sizeof(void*),1)))))));}if(
system((const char*)_untag_dyneither_ptr(_tmp100,sizeof(char),1))!= 0){Cyc_compile_failure=
1;{const char*_tmp480;void*_tmp47F;(_tmp47F=0,Cyc_fprintf(Cyc_stderr,((_tmp480="\nError: preprocessing\n",
_tag_dyneither(_tmp480,sizeof(char),23))),_tag_dyneither(_tmp47F,sizeof(void*),0)));}
return;}if(Cyc_stop_after_cpp_r)return;Cyc_Position_reset_position((struct
_dyneither_ptr)_tmpE6);{const char*_tmp482;const char*_tmp481;struct Cyc___cycFILE*
in_file=Cyc_try_file_open((struct _dyneither_ptr)_tmpE6,((_tmp481="r",
_tag_dyneither(_tmp481,sizeof(char),2))),((_tmp482="file",_tag_dyneither(_tmp482,
sizeof(char),5))));if(Cyc_compile_failure)return;{struct Cyc_List_List*tds=0;{
struct _handler_cons _tmp106;_push_handler(& _tmp106);{int _tmp108=0;if(setjmp(
_tmp106.handler))_tmp108=1;if(!_tmp108){{const char*_tmp483;tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,
struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp483="parsing",_tag_dyneither(_tmp483,sizeof(char),8))),Cyc_do_parse,(struct
Cyc___cycFILE*)_check_null(in_file),tds);};_pop_handler();}else{void*_tmp107=(
void*)_exn_thrown;void*_tmp10B=_tmp107;_LL27:;_LL28: Cyc_file_close((struct Cyc___cycFILE*)
_check_null(in_file));Cyc_remove_file((struct _dyneither_ptr)_tmpE6);(int)_throw(
_tmp10B);_LL29:;_LL2A:(void)_throw(_tmp10B);_LL26:;}};}Cyc_file_close((struct Cyc___cycFILE*)
in_file);if(Cyc_compile_failure){Cyc_remove_file((struct _dyneither_ptr)_tmpE6);
return;}if(Cyc_Absyn_porting_c_code){Cyc_Port_port(tds);return;}{struct
_RegionHandle _tmp10C=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmp10C;
_push_region(tc_rgn);{struct Cyc_Tcenv_Tenv*_tmp10D=Cyc_Tcenv_tc_init(tc_rgn);if(
Cyc_parseonly_r)goto PRINTC;{struct _tuple20 _tmp484;struct _tuple20 _tmp10E=(_tmp484.f1=
tc_rgn,((_tmp484.f2=_tmp10D,_tmp484)));{const char*_tmp485;tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple20*,struct
Cyc_List_List*),struct _tuple20*env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp485="type checking",_tag_dyneither(_tmp485,sizeof(char),14))),Cyc_do_typecheck,&
_tmp10E,tds);}if(Cyc_compile_failure){Cyc_remove_file((struct _dyneither_ptr)
_tmpE6);_npop_handler(0);return;}if(Cyc_tc_r)goto PRINTC;{const char*_tmp486;tds=((
struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(
int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp486="control-flow checking",_tag_dyneither(_tmp486,sizeof(char),22))),Cyc_do_cfcheck,
1,tds);}if(Cyc_compile_failure){Cyc_remove_file((struct _dyneither_ptr)_tmpE6);
_npop_handler(0);return;}if(Cyc_ic_r){struct Cyc___cycFILE*inter_file=Cyc_fopen((
const char*)((char*)_untag_dyneither_ptr(_tmpE7,sizeof(char),1)),(const char*)"r");
const char*_tmp488;const char*_tmp487;struct Cyc___cycFILE*inter_objfile=Cyc_try_file_open((
struct _dyneither_ptr)_tmpE8,((_tmp487="w",_tag_dyneither(_tmp487,sizeof(char),2))),((
_tmp488="interface object file",_tag_dyneither(_tmp488,sizeof(char),22))));if(
inter_objfile == 0){Cyc_compile_failure=1;_npop_handler(0);return;}Cyc_Position_reset_position((
struct _dyneither_ptr)_tmpE7);{struct _tuple21 _tmp489;struct _tuple21 _tmp111=(
_tmp489.f1=_tmp10D,((_tmp489.f2=inter_file,((_tmp489.f3=(struct Cyc___cycFILE*)
inter_objfile,_tmp489)))));{const char*_tmp48A;tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple21*,struct
Cyc_List_List*),struct _tuple21*env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp48A="interface checking",_tag_dyneither(_tmp48A,sizeof(char),19))),Cyc_do_interface,&
_tmp111,tds);}if(inter_file != 0)Cyc_file_close((struct Cyc___cycFILE*)inter_file);
Cyc_file_close((struct Cyc___cycFILE*)inter_objfile);};}};}if(Cyc_cf_r)goto PRINTC;{
const char*_tmp48B;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,
struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*
tds))Cyc_do_stage)(((_tmp48B="translation to C",_tag_dyneither(_tmp48B,sizeof(
char),17))),Cyc_do_translate,1,tds);}if(Cyc_compile_failure){Cyc_remove_file((
struct _dyneither_ptr)_tmpE6);_npop_handler(0);return;}if(!Cyc_compile_failure)
Cyc_Tcutil_flush_warnings();Cyc_remove_file((struct _dyneither_ptr)_tmpE6);if(Cyc_compile_failure){
_npop_handler(0);return;}if(Cyc_Cyclone_tovc_r  || Cyc_elim_se_r){const char*
_tmp48C;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*
f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp48C="post-pass to VC",_tag_dyneither(_tmp48C,sizeof(char),16))),Cyc_do_tovc,
1,tds);}if(Cyc_compile_failure){_npop_handler(0);return;}PRINTC: {struct Cyc___cycFILE*
out_file;if((Cyc_parseonly_r  || Cyc_tc_r) || Cyc_cf_r){if(Cyc_output_file != 0){
const char*_tmp48E;const char*_tmp48D;out_file=Cyc_try_file_open(*((struct
_dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp48D="w",_tag_dyneither(
_tmp48D,sizeof(char),2))),((_tmp48E="output file",_tag_dyneither(_tmp48E,sizeof(
char),12))));}else{out_file=(struct Cyc___cycFILE*)Cyc_stdout;}}else{if(Cyc_toc_r
 && Cyc_output_file != 0){const char*_tmp490;const char*_tmp48F;out_file=Cyc_try_file_open(*((
struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp48F="w",_tag_dyneither(
_tmp48F,sizeof(char),2))),((_tmp490="output file",_tag_dyneither(_tmp490,sizeof(
char),12))));}else{const char*_tmp492;const char*_tmp491;out_file=Cyc_try_file_open((
struct _dyneither_ptr)_tmpE9,((_tmp491="w",_tag_dyneither(_tmp491,sizeof(char),2))),((
_tmp492="output file",_tag_dyneither(_tmp492,sizeof(char),12))));}}if(Cyc_compile_failure
 || !((unsigned int)out_file)){_npop_handler(0);return;}{struct _handler_cons
_tmp11F;_push_handler(& _tmp11F);{int _tmp121=0;if(setjmp(_tmp11F.handler))_tmp121=
1;if(!_tmp121){if(!Cyc_noprint_r){const char*_tmp493;tds=((struct Cyc_List_List*(*)(
struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,
struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp493="printing",_tag_dyneither(_tmp493,sizeof(char),9))),Cyc_do_print,(struct
Cyc___cycFILE*)out_file,tds);};_pop_handler();}else{void*_tmp120=(void*)
_exn_thrown;void*_tmp124=_tmp120;_LL2C:;_LL2D: Cyc_compile_failure=1;Cyc_file_close((
struct Cyc___cycFILE*)out_file);{struct _dyneither_ptr*_tmp496;struct Cyc_List_List*
_tmp495;Cyc_cfiles=((_tmp495=_cycalloc(sizeof(*_tmp495)),((_tmp495->hd=((_tmp496=
_cycalloc(sizeof(*_tmp496)),((_tmp496[0]=(struct _dyneither_ptr)_tmpE9,_tmp496)))),((
_tmp495->tl=Cyc_cfiles,_tmp495))))));}(int)_throw(_tmp124);_LL2E:;_LL2F:(void)
_throw(_tmp124);_LL2B:;}};}Cyc_file_close((struct Cyc___cycFILE*)out_file);{
struct _dyneither_ptr*_tmp499;struct Cyc_List_List*_tmp498;Cyc_cfiles=((_tmp498=
_cycalloc(sizeof(*_tmp498)),((_tmp498->hd=((_tmp499=_cycalloc(sizeof(*_tmp499)),((
_tmp499[0]=(struct _dyneither_ptr)_tmpE9,_tmp499)))),((_tmp498->tl=Cyc_cfiles,
_tmp498))))));};};_pop_region(tc_rgn);};};};};};};};};};}static char _tmp141[8]="<final>";
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
const char*_tmp49A;struct _dyneither_ptr _tmp148=Cyc_strconcat((struct
_dyneither_ptr)basename,((_tmp49A=".cycio",_tag_dyneither(_tmp49A,sizeof(char),7))));
const char*_tmp49C;const char*_tmp49B;struct Cyc___cycFILE*_tmp149=Cyc_try_file_open((
struct _dyneither_ptr)_tmp148,((_tmp49B="rb",_tag_dyneither(_tmp49B,sizeof(char),
3))),((_tmp49C="interface object file",_tag_dyneither(_tmp49C,sizeof(char),22))));
if(_tmp149 == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();exit(1);}Cyc_Position_reset_position((
struct _dyneither_ptr)_tmp148);{struct Cyc_Interface_I*_tmp14A=Cyc_Interface_load((
struct Cyc___cycFILE*)_tmp149);Cyc_file_close((struct Cyc___cycFILE*)_tmp149);
return _tmp14A;};};};}static int Cyc_is_cfile(struct _dyneither_ptr*n);static int Cyc_is_cfile(
struct _dyneither_ptr*n){return*((const char*)_check_dyneither_subscript(*n,
sizeof(char),0))!= '-';}void GC_blacklist_warn_clear();struct _tuple22{struct
_dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
int Cyc_main(int argc,struct _dyneither_ptr argv);static void _tmp4A2(unsigned int*
_tmp4A1,unsigned int*_tmp4A0,int**_tmp49E){for(*_tmp4A1=0;*_tmp4A1 < *_tmp4A0;(*
_tmp4A1)++){(*_tmp49E)[*_tmp4A1]=0;}}static void _tmp4AA(unsigned int*_tmp4A9,
unsigned int*_tmp4A8,struct _dyneither_ptr**_tmp4A6){for(*_tmp4A9=0;*_tmp4A9 < *
_tmp4A8;(*_tmp4A9)++){(*_tmp4A6)[*_tmp4A9]=(struct _dyneither_ptr)_tag_dyneither(
0,0,0);}}static void _tmp4B0(unsigned int*_tmp4AF,unsigned int*_tmp4AE,struct
_dyneither_ptr**_tmp4AC){for(*_tmp4AF=0;*_tmp4AF < *_tmp4AE;(*_tmp4AF)++){(*
_tmp4AC)[*_tmp4AF]=(struct _dyneither_ptr)_tag_dyneither(0,0,0);}}int Cyc_main(int
argc,struct _dyneither_ptr argv){GC_blacklist_warn_clear();{struct Cyc_List_List*
cyclone_arch_path;struct _dyneither_ptr def_lib_path;struct _dyneither_ptr comp=(
struct _dyneither_ptr)Cstring_to_string(Ccomp);struct _RegionHandle _tmp14E=
_new_region("r");struct _RegionHandle*r=& _tmp14E;_push_region(r);{unsigned int
_tmp4A1;unsigned int _tmp4A0;struct _dyneither_ptr _tmp49F;int*_tmp49E;unsigned int
_tmp49D;struct _dyneither_ptr _tmp14F=(_tmp49D=(unsigned int)argc,((_tmp49E=(int*)
_cycalloc_atomic(_check_times(sizeof(int),_tmp49D)),((_tmp49F=_tag_dyneither(
_tmp49E,sizeof(int),_tmp49D),((((_tmp4A0=_tmp49D,_tmp4A2(& _tmp4A1,& _tmp4A0,&
_tmp49E))),_tmp49F)))))));int _tmp150=0;int i;int j;int k;for(i=1;i < argc;++ i){const
char*_tmp4A3;if(Cyc_strncmp(((_tmp4A3="-B",_tag_dyneither(_tmp4A3,sizeof(char),3))),(
struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,
sizeof(struct _dyneither_ptr),i)),2)== 0){*((int*)_check_dyneither_subscript(
_tmp14F,sizeof(int),i))=1;++ _tmp150;}else{const char*_tmp4A4;if(Cyc_strcmp(((
_tmp4A4="-b",_tag_dyneither(_tmp4A4,sizeof(char),3))),(struct _dyneither_ptr)*((
struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct
_dyneither_ptr),i)))== 0){*((int*)_check_dyneither_subscript(_tmp14F,sizeof(int),
i))=1;++ _tmp150;++ i;if(i >= argc)break;*((int*)_check_dyneither_subscript(_tmp14F,
sizeof(int),i))=1;++ _tmp150;}}}{unsigned int _tmp4A9;unsigned int _tmp4A8;struct
_dyneither_ptr _tmp4A7;struct _dyneither_ptr*_tmp4A6;unsigned int _tmp4A5;struct
_dyneither_ptr _tmp153=(_tmp4A5=(unsigned int)(_tmp150 + 1),((_tmp4A6=(struct
_dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp4A5)),((
_tmp4A7=_tag_dyneither(_tmp4A6,sizeof(struct _dyneither_ptr),_tmp4A5),((((_tmp4A8=
_tmp4A5,_tmp4AA(& _tmp4A9,& _tmp4A8,& _tmp4A6))),_tmp4A7)))))));unsigned int _tmp4AF;
unsigned int _tmp4AE;struct _dyneither_ptr _tmp4AD;struct _dyneither_ptr*_tmp4AC;
unsigned int _tmp4AB;struct _dyneither_ptr _tmp154=(_tmp4AB=(unsigned int)(argc - 
_tmp150),((_tmp4AC=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct
_dyneither_ptr),_tmp4AB)),((_tmp4AD=_tag_dyneither(_tmp4AC,sizeof(struct
_dyneither_ptr),_tmp4AB),((((_tmp4AE=_tmp4AB,_tmp4B0(& _tmp4AF,& _tmp4AE,& _tmp4AC))),
_tmp4AD)))))));*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp153,
sizeof(struct _dyneither_ptr),0))=(*((struct _dyneither_ptr*)
_check_dyneither_subscript(_tmp154,sizeof(struct _dyneither_ptr),0))=*((struct
_dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),0)));
for(i=(j=(k=1));i < argc;++ i){if(*((int*)_check_dyneither_subscript(_tmp14F,
sizeof(int),i)))*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp153,
sizeof(struct _dyneither_ptr),j ++))=*((struct _dyneither_ptr*)
_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));else{*((struct
_dyneither_ptr*)_check_dyneither_subscript(_tmp154,sizeof(struct _dyneither_ptr),
k ++))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct
_dyneither_ptr),i));}}{struct _tuple22*_tmp931;const char*_tmp930;struct Cyc_Arg_Set_spec_struct
_tmp92F;struct Cyc_Arg_Set_spec_struct*_tmp92E;const char*_tmp92D;const char*
_tmp92C;struct _tuple22*_tmp92B;const char*_tmp92A;struct Cyc_Arg_Unit_spec_struct
_tmp929;struct Cyc_Arg_Unit_spec_struct*_tmp928;const char*_tmp927;const char*
_tmp926;struct _tuple22*_tmp925;const char*_tmp924;struct Cyc_Arg_Clear_spec_struct
_tmp923;struct Cyc_Arg_Clear_spec_struct*_tmp922;const char*_tmp921;const char*
_tmp920;struct _tuple22*_tmp91F;const char*_tmp91E;struct Cyc_Arg_Set_spec_struct
_tmp91D;struct Cyc_Arg_Set_spec_struct*_tmp91C;const char*_tmp91B;const char*
_tmp91A;struct _tuple22*_tmp919;const char*_tmp918;struct Cyc_Arg_Set_spec_struct
_tmp917;struct Cyc_Arg_Set_spec_struct*_tmp916;const char*_tmp915;const char*
_tmp914;struct _tuple22*_tmp913;const char*_tmp912;struct Cyc_Arg_Set_spec_struct
_tmp911;struct Cyc_Arg_Set_spec_struct*_tmp910;const char*_tmp90F;const char*
_tmp90E;struct _tuple22*_tmp90D;const char*_tmp90C;struct Cyc_Arg_String_spec_struct
_tmp90B;struct Cyc_Arg_String_spec_struct*_tmp90A;const char*_tmp909;const char*
_tmp908;struct _tuple22*_tmp907;const char*_tmp906;struct Cyc_Arg_Set_spec_struct
_tmp905;struct Cyc_Arg_Set_spec_struct*_tmp904;const char*_tmp903;const char*
_tmp902;struct _tuple22*_tmp901;const char*_tmp900;struct Cyc_Arg_Set_spec_struct
_tmp8FF;struct Cyc_Arg_Set_spec_struct*_tmp8FE;const char*_tmp8FD;const char*
_tmp8FC;struct _tuple22*_tmp8FB;const char*_tmp8FA;struct Cyc_Arg_Set_spec_struct
_tmp8F9;struct Cyc_Arg_Set_spec_struct*_tmp8F8;const char*_tmp8F7;const char*
_tmp8F6;struct _tuple22*_tmp8F5;const char*_tmp8F4;struct Cyc_Arg_Set_spec_struct
_tmp8F3;struct Cyc_Arg_Set_spec_struct*_tmp8F2;const char*_tmp8F1;const char*
_tmp8F0;struct _tuple22*_tmp8EF;const char*_tmp8EE;struct Cyc_Arg_Set_spec_struct
_tmp8ED;struct Cyc_Arg_Set_spec_struct*_tmp8EC;const char*_tmp8EB;const char*
_tmp8EA;struct _tuple22*_tmp8E9;const char*_tmp8E8;struct Cyc_Arg_Set_spec_struct
_tmp8E7;struct Cyc_Arg_Set_spec_struct*_tmp8E6;const char*_tmp8E5;const char*
_tmp8E4;struct _tuple22*_tmp8E3;const char*_tmp8E2;struct Cyc_Arg_Set_spec_struct
_tmp8E1;struct Cyc_Arg_Set_spec_struct*_tmp8E0;const char*_tmp8DF;const char*
_tmp8DE;struct _tuple22*_tmp8DD;const char*_tmp8DC;struct Cyc_Arg_Set_spec_struct
_tmp8DB;struct Cyc_Arg_Set_spec_struct*_tmp8DA;const char*_tmp8D9;const char*
_tmp8D8;struct _tuple22*_tmp8D7;const char*_tmp8D6;struct Cyc_Arg_Unit_spec_struct
_tmp8D5;struct Cyc_Arg_Unit_spec_struct*_tmp8D4;const char*_tmp8D3;const char*
_tmp8D2;struct _tuple22*_tmp8D1;const char*_tmp8D0;struct Cyc_Arg_Unit_spec_struct
_tmp8CF;struct Cyc_Arg_Unit_spec_struct*_tmp8CE;const char*_tmp8CD;const char*
_tmp8CC;struct _tuple22*_tmp8CB;const char*_tmp8CA;struct Cyc_Arg_String_spec_struct
_tmp8C9;struct Cyc_Arg_String_spec_struct*_tmp8C8;const char*_tmp8C7;const char*
_tmp8C6;struct _tuple22*_tmp8C5;const char*_tmp8C4;struct Cyc_Arg_Unit_spec_struct
_tmp8C3;struct Cyc_Arg_Unit_spec_struct*_tmp8C2;const char*_tmp8C1;const char*
_tmp8C0;struct _tuple22*_tmp8BF;const char*_tmp8BE;struct Cyc_Arg_Unit_spec_struct
_tmp8BD;struct Cyc_Arg_Unit_spec_struct*_tmp8BC;const char*_tmp8BB;const char*
_tmp8BA;struct _tuple22*_tmp8B9;const char*_tmp8B8;struct Cyc_Arg_Unit_spec_struct
_tmp8B7;struct Cyc_Arg_Unit_spec_struct*_tmp8B6;const char*_tmp8B5;const char*
_tmp8B4;struct _tuple22*_tmp8B3;const char*_tmp8B2;struct Cyc_Arg_Unit_spec_struct
_tmp8B1;struct Cyc_Arg_Unit_spec_struct*_tmp8B0;const char*_tmp8AF;const char*
_tmp8AE;struct _tuple22*_tmp8AD;const char*_tmp8AC;struct Cyc_Arg_Unit_spec_struct
_tmp8AB;struct Cyc_Arg_Unit_spec_struct*_tmp8AA;const char*_tmp8A9;const char*
_tmp8A8;struct _tuple22*_tmp8A7;const char*_tmp8A6;struct Cyc_Arg_Set_spec_struct
_tmp8A5;struct Cyc_Arg_Set_spec_struct*_tmp8A4;const char*_tmp8A3;const char*
_tmp8A2;struct _tuple22*_tmp8A1;const char*_tmp8A0;struct Cyc_Arg_Unit_spec_struct
_tmp89F;struct Cyc_Arg_Unit_spec_struct*_tmp89E;const char*_tmp89D;const char*
_tmp89C;struct _tuple22*_tmp89B;const char*_tmp89A;struct Cyc_Arg_String_spec_struct
_tmp899;struct Cyc_Arg_String_spec_struct*_tmp898;const char*_tmp897;const char*
_tmp896;struct _tuple22*_tmp895;const char*_tmp894;struct Cyc_Arg_Unit_spec_struct
_tmp893;struct Cyc_Arg_Unit_spec_struct*_tmp892;const char*_tmp891;const char*
_tmp890;struct _tuple22*_tmp88F;const char*_tmp88E;struct Cyc_Arg_Set_spec_struct
_tmp88D;struct Cyc_Arg_Set_spec_struct*_tmp88C;const char*_tmp88B;const char*
_tmp88A;struct _tuple22*_tmp889;const char*_tmp888;struct Cyc_Arg_Clear_spec_struct
_tmp887;struct Cyc_Arg_Clear_spec_struct*_tmp886;const char*_tmp885;const char*
_tmp884;struct _tuple22*_tmp883;const char*_tmp882;struct Cyc_Arg_Set_spec_struct
_tmp881;struct Cyc_Arg_Set_spec_struct*_tmp880;const char*_tmp87F;const char*
_tmp87E;struct _tuple22*_tmp87D;const char*_tmp87C;struct Cyc_Arg_Set_spec_struct
_tmp87B;struct Cyc_Arg_Set_spec_struct*_tmp87A;const char*_tmp879;const char*
_tmp878;struct _tuple22*_tmp877;const char*_tmp876;struct Cyc_Arg_Set_spec_struct
_tmp875;struct Cyc_Arg_Set_spec_struct*_tmp874;const char*_tmp873;const char*
_tmp872;struct _tuple22*_tmp871;const char*_tmp870;struct Cyc_Arg_Set_spec_struct
_tmp86F;struct Cyc_Arg_Set_spec_struct*_tmp86E;const char*_tmp86D;const char*
_tmp86C;struct _tuple22*_tmp86B;const char*_tmp86A;struct Cyc_Arg_Set_spec_struct
_tmp869;struct Cyc_Arg_Set_spec_struct*_tmp868;const char*_tmp867;const char*
_tmp866;struct _tuple22*_tmp865;const char*_tmp864;struct Cyc_Arg_Set_spec_struct
_tmp863;struct Cyc_Arg_Set_spec_struct*_tmp862;const char*_tmp861;const char*
_tmp860;struct _tuple22*_tmp85F;const char*_tmp85E;struct Cyc_Arg_Set_spec_struct
_tmp85D;struct Cyc_Arg_Set_spec_struct*_tmp85C;const char*_tmp85B;const char*
_tmp85A;struct _tuple22*_tmp859;const char*_tmp858;struct Cyc_Arg_Set_spec_struct
_tmp857;struct Cyc_Arg_Set_spec_struct*_tmp856;const char*_tmp855;const char*
_tmp854;struct _tuple22*_tmp853;const char*_tmp852;struct Cyc_Arg_Unit_spec_struct
_tmp851;struct Cyc_Arg_Unit_spec_struct*_tmp850;const char*_tmp84F;const char*
_tmp84E;struct _tuple22*_tmp84D;const char*_tmp84C;struct Cyc_Arg_String_spec_struct
_tmp84B;struct Cyc_Arg_String_spec_struct*_tmp84A;const char*_tmp849;const char*
_tmp848;struct _tuple22*_tmp847;const char*_tmp846;struct Cyc_Arg_String_spec_struct
_tmp845;struct Cyc_Arg_String_spec_struct*_tmp844;const char*_tmp843;const char*
_tmp842;struct _tuple22*_tmp841;const char*_tmp840;struct Cyc_Arg_Flag_spec_struct
_tmp83F;struct Cyc_Arg_Flag_spec_struct*_tmp83E;const char*_tmp83D;const char*
_tmp83C;struct _tuple22*_tmp83B;const char*_tmp83A;struct Cyc_Arg_Unit_spec_struct
_tmp839;struct Cyc_Arg_Unit_spec_struct*_tmp838;const char*_tmp837;const char*
_tmp836;struct _tuple22*_tmp835;const char*_tmp834;struct Cyc_Arg_Unit_spec_struct
_tmp833;struct Cyc_Arg_Unit_spec_struct*_tmp832;const char*_tmp831;const char*
_tmp830;struct _tuple22*_tmp82F;const char*_tmp82E;struct Cyc_Arg_Unit_spec_struct
_tmp82D;struct Cyc_Arg_Unit_spec_struct*_tmp82C;const char*_tmp82B;const char*
_tmp82A;struct _tuple22*_tmp829;const char*_tmp828;struct Cyc_Arg_Unit_spec_struct
_tmp827;struct Cyc_Arg_Unit_spec_struct*_tmp826;const char*_tmp825;const char*
_tmp824;struct _tuple22*_tmp823;const char*_tmp822;struct Cyc_Arg_Flag_spec_struct
_tmp821;struct Cyc_Arg_Flag_spec_struct*_tmp820;const char*_tmp81F;const char*
_tmp81E;struct _tuple22*_tmp81D;const char*_tmp81C;struct Cyc_Arg_Flag_spec_struct
_tmp81B;struct Cyc_Arg_Flag_spec_struct*_tmp81A;const char*_tmp819;const char*
_tmp818;struct _tuple22*_tmp817;const char*_tmp816;struct Cyc_Arg_Flag_spec_struct
_tmp815;struct Cyc_Arg_Flag_spec_struct*_tmp814;const char*_tmp813;const char*
_tmp812;struct _tuple22*_tmp811;const char*_tmp810;struct Cyc_Arg_Flag_spec_struct
_tmp80F;struct Cyc_Arg_Flag_spec_struct*_tmp80E;const char*_tmp80D;const char*
_tmp80C;struct _tuple22*_tmp80B;const char*_tmp80A;struct Cyc_Arg_Flag_spec_struct
_tmp809;struct Cyc_Arg_Flag_spec_struct*_tmp808;const char*_tmp807;const char*
_tmp806;struct _tuple22*_tmp805;const char*_tmp804;struct Cyc_Arg_Flag_spec_struct
_tmp803;struct Cyc_Arg_Flag_spec_struct*_tmp802;const char*_tmp801;const char*
_tmp800;struct _tuple22*_tmp7FF;const char*_tmp7FE;struct Cyc_Arg_Flag_spec_struct
_tmp7FD;struct Cyc_Arg_Flag_spec_struct*_tmp7FC;const char*_tmp7FB;const char*
_tmp7FA;struct _tuple22*_tmp7F9;const char*_tmp7F8;struct Cyc_Arg_Flag_spec_struct
_tmp7F7;struct Cyc_Arg_Flag_spec_struct*_tmp7F6;const char*_tmp7F5;const char*
_tmp7F4;struct _tuple22*_tmp7F3;const char*_tmp7F2;struct Cyc_Arg_String_spec_struct
_tmp7F1;struct Cyc_Arg_String_spec_struct*_tmp7F0;const char*_tmp7EF;const char*
_tmp7EE;struct _tuple22*_tmp7ED;const char*_tmp7EC;struct Cyc_Arg_Unit_spec_struct
_tmp7EB;struct Cyc_Arg_Unit_spec_struct*_tmp7EA;const char*_tmp7E9;const char*
_tmp7E8;struct _tuple22*_tmp7E7;const char*_tmp7E6;struct Cyc_Arg_Flag_spec_struct
_tmp7E5;struct Cyc_Arg_Flag_spec_struct*_tmp7E4;const char*_tmp7E3;const char*
_tmp7E2;struct _tuple22*_tmp7E1;const char*_tmp7E0;struct Cyc_Arg_Flag_spec_struct
_tmp7DF;struct Cyc_Arg_Flag_spec_struct*_tmp7DE;const char*_tmp7DD;const char*
_tmp7DC;struct _tuple22*_tmp7DB;const char*_tmp7DA;struct Cyc_Arg_Flag_spec_struct
_tmp7D9;struct Cyc_Arg_Flag_spec_struct*_tmp7D8;const char*_tmp7D7;const char*
_tmp7D6;struct _tuple22*_tmp7D5;const char*_tmp7D4;struct Cyc_Arg_Flag_spec_struct
_tmp7D3;struct Cyc_Arg_Flag_spec_struct*_tmp7D2;const char*_tmp7D1;const char*
_tmp7D0;struct _tuple22*_tmp7CF;const char*_tmp7CE;struct Cyc_Arg_Flag_spec_struct
_tmp7CD;struct Cyc_Arg_Flag_spec_struct*_tmp7CC;const char*_tmp7CB;const char*
_tmp7CA;struct _tuple22*_tmp7C9;const char*_tmp7C8;struct Cyc_Arg_Flag_spec_struct
_tmp7C7;struct Cyc_Arg_Flag_spec_struct*_tmp7C6;const char*_tmp7C5;const char*
_tmp7C4;struct _tuple22*_tmp7C3;const char*_tmp7C2;struct Cyc_Arg_String_spec_struct
_tmp7C1;struct Cyc_Arg_String_spec_struct*_tmp7C0;const char*_tmp7BF;const char*
_tmp7BE;struct _tuple22*_tmp7BD;const char*_tmp7BC;struct Cyc_Arg_Unit_spec_struct
_tmp7BB;struct Cyc_Arg_Unit_spec_struct*_tmp7BA;const char*_tmp7B9;const char*
_tmp7B8;struct _tuple22*_tmp7B7;const char*_tmp7B6;struct Cyc_Arg_Set_spec_struct
_tmp7B5;struct Cyc_Arg_Set_spec_struct*_tmp7B4;const char*_tmp7B3;const char*
_tmp7B2;struct _tuple22*_tmp7B1[64];struct Cyc_List_List*options=(_tmp7B1[63]=((
_tmp931=_region_malloc(r,sizeof(*_tmp931)),((_tmp931->f1=((_tmp92C="-noregions",
_tag_dyneither(_tmp92C,sizeof(char),11))),((_tmp931->f2=0,((_tmp931->f3=((
_tmp92D="",_tag_dyneither(_tmp92D,sizeof(char),1))),((_tmp931->f4=(void*)((
_tmp92E=_region_malloc(r,sizeof(*_tmp92E)),((_tmp92E[0]=((_tmp92F.tag=3,((
_tmp92F.f1=& Cyc_Absyn_no_regions,_tmp92F)))),_tmp92E)))),((_tmp931->f5=((_tmp930="Generate code that doesn't use regions",
_tag_dyneither(_tmp930,sizeof(char),39))),_tmp931)))))))))))),((_tmp7B1[62]=((
_tmp92B=_region_malloc(r,sizeof(*_tmp92B)),((_tmp92B->f1=((_tmp926="-port",
_tag_dyneither(_tmp926,sizeof(char),6))),((_tmp92B->f2=0,((_tmp92B->f3=((_tmp927="",
_tag_dyneither(_tmp927,sizeof(char),1))),((_tmp92B->f4=(void*)((_tmp928=
_region_malloc(r,sizeof(*_tmp928)),((_tmp928[0]=((_tmp929.tag=0,((_tmp929.f1=Cyc_set_port_c_code,
_tmp929)))),_tmp928)))),((_tmp92B->f5=((_tmp92A="Suggest how to port C code to Cyclone",
_tag_dyneither(_tmp92A,sizeof(char),38))),_tmp92B)))))))))))),((_tmp7B1[61]=((
_tmp925=_region_malloc(r,sizeof(*_tmp925)),((_tmp925->f1=((_tmp920="-detailedlocation",
_tag_dyneither(_tmp920,sizeof(char),18))),((_tmp925->f2=0,((_tmp925->f3=((
_tmp921="",_tag_dyneither(_tmp921,sizeof(char),1))),((_tmp925->f4=(void*)((
_tmp922=_region_malloc(r,sizeof(*_tmp922)),((_tmp922[0]=((_tmp923.tag=4,((
_tmp923.f1=& Cyc_Position_use_gcc_style_location,_tmp923)))),_tmp922)))),((
_tmp925->f5=((_tmp924="Try to give more detailed location information for errors",
_tag_dyneither(_tmp924,sizeof(char),58))),_tmp925)))))))))))),((_tmp7B1[60]=((
_tmp91F=_region_malloc(r,sizeof(*_tmp91F)),((_tmp91F->f1=((_tmp91A="-noregister",
_tag_dyneither(_tmp91A,sizeof(char),12))),((_tmp91F->f2=0,((_tmp91F->f3=((
_tmp91B="",_tag_dyneither(_tmp91B,sizeof(char),1))),((_tmp91F->f4=(void*)((
_tmp91C=_region_malloc(r,sizeof(*_tmp91C)),((_tmp91C[0]=((_tmp91D.tag=3,((
_tmp91D.f1=& Cyc_Parse_no_register,_tmp91D)))),_tmp91C)))),((_tmp91F->f5=((
_tmp91E="Treat register storage class as public",_tag_dyneither(_tmp91E,sizeof(
char),39))),_tmp91F)))))))))))),((_tmp7B1[59]=((_tmp919=_region_malloc(r,sizeof(*
_tmp919)),((_tmp919->f1=((_tmp914="-warnnullchecks",_tag_dyneither(_tmp914,
sizeof(char),16))),((_tmp919->f2=0,((_tmp919->f3=((_tmp915="",_tag_dyneither(
_tmp915,sizeof(char),1))),((_tmp919->f4=(void*)((_tmp916=_region_malloc(r,
sizeof(*_tmp916)),((_tmp916[0]=((_tmp917.tag=3,((_tmp917.f1=& Cyc_Toc_warn_all_null_deref,
_tmp917)))),_tmp916)))),((_tmp919->f5=((_tmp918="Warn when any null check can't be eliminated",
_tag_dyneither(_tmp918,sizeof(char),45))),_tmp919)))))))))))),((_tmp7B1[58]=((
_tmp913=_region_malloc(r,sizeof(*_tmp913)),((_tmp913->f1=((_tmp90E="-warnboundschecks",
_tag_dyneither(_tmp90E,sizeof(char),18))),((_tmp913->f2=0,((_tmp913->f3=((
_tmp90F="",_tag_dyneither(_tmp90F,sizeof(char),1))),((_tmp913->f4=(void*)((
_tmp910=_region_malloc(r,sizeof(*_tmp910)),((_tmp910[0]=((_tmp911.tag=3,((
_tmp911.f1=& Cyc_Toc_warn_bounds_checks,_tmp911)))),_tmp910)))),((_tmp913->f5=((
_tmp912="Warn when bounds checks can't be eliminated",_tag_dyneither(_tmp912,
sizeof(char),44))),_tmp913)))))))))))),((_tmp7B1[57]=((_tmp90D=_region_malloc(r,
sizeof(*_tmp90D)),((_tmp90D->f1=((_tmp908="-CI",_tag_dyneither(_tmp908,sizeof(
char),4))),((_tmp90D->f2=0,((_tmp90D->f3=((_tmp909=" <file>",_tag_dyneither(
_tmp909,sizeof(char),8))),((_tmp90D->f4=(void*)((_tmp90A=_region_malloc(r,
sizeof(*_tmp90A)),((_tmp90A[0]=((_tmp90B.tag=5,((_tmp90B.f1=Cyc_set_cyc_include,
_tmp90B)))),_tmp90A)))),((_tmp90D->f5=((_tmp90C="Set cyc_include.h to be <file>",
_tag_dyneither(_tmp90C,sizeof(char),31))),_tmp90D)))))))))))),((_tmp7B1[56]=((
_tmp907=_region_malloc(r,sizeof(*_tmp907)),((_tmp907->f1=((_tmp902="-nocyc_setjmp",
_tag_dyneither(_tmp902,sizeof(char),14))),((_tmp907->f2=0,((_tmp907->f3=((
_tmp903="",_tag_dyneither(_tmp903,sizeof(char),1))),((_tmp907->f4=(void*)((
_tmp904=_region_malloc(r,sizeof(*_tmp904)),((_tmp904[0]=((_tmp905.tag=3,((
_tmp905.f1=& Cyc_nocyc_setjmp_r,_tmp905)))),_tmp904)))),((_tmp907->f5=((_tmp906="Do not use compiler special file cyc_setjmp.h",
_tag_dyneither(_tmp906,sizeof(char),46))),_tmp907)))))))))))),((_tmp7B1[55]=((
_tmp901=_region_malloc(r,sizeof(*_tmp901)),((_tmp901->f1=((_tmp8FC="-printalleffects",
_tag_dyneither(_tmp8FC,sizeof(char),17))),((_tmp901->f2=0,((_tmp901->f3=((
_tmp8FD="",_tag_dyneither(_tmp8FD,sizeof(char),1))),((_tmp901->f4=(void*)((
_tmp8FE=_region_malloc(r,sizeof(*_tmp8FE)),((_tmp8FE[0]=((_tmp8FF.tag=3,((
_tmp8FF.f1=& Cyc_print_all_effects_r,_tmp8FF)))),_tmp8FE)))),((_tmp901->f5=((
_tmp900="Print effects for functions (type debugging)",_tag_dyneither(_tmp900,
sizeof(char),45))),_tmp901)))))))))))),((_tmp7B1[54]=((_tmp8FB=_region_malloc(r,
sizeof(*_tmp8FB)),((_tmp8FB->f1=((_tmp8F6="-printfullevars",_tag_dyneither(
_tmp8F6,sizeof(char),16))),((_tmp8FB->f2=0,((_tmp8FB->f3=((_tmp8F7="",
_tag_dyneither(_tmp8F7,sizeof(char),1))),((_tmp8FB->f4=(void*)((_tmp8F8=
_region_malloc(r,sizeof(*_tmp8F8)),((_tmp8F8[0]=((_tmp8F9.tag=3,((_tmp8F9.f1=&
Cyc_print_full_evars_r,_tmp8F9)))),_tmp8F8)))),((_tmp8FB->f5=((_tmp8FA="Print full information for evars (type debugging)",
_tag_dyneither(_tmp8FA,sizeof(char),50))),_tmp8FB)))))))))))),((_tmp7B1[53]=((
_tmp8F5=_region_malloc(r,sizeof(*_tmp8F5)),((_tmp8F5->f1=((_tmp8F0="-printallkinds",
_tag_dyneither(_tmp8F0,sizeof(char),15))),((_tmp8F5->f2=0,((_tmp8F5->f3=((
_tmp8F1="",_tag_dyneither(_tmp8F1,sizeof(char),1))),((_tmp8F5->f4=(void*)((
_tmp8F2=_region_malloc(r,sizeof(*_tmp8F2)),((_tmp8F2[0]=((_tmp8F3.tag=3,((
_tmp8F3.f1=& Cyc_print_all_kinds_r,_tmp8F3)))),_tmp8F2)))),((_tmp8F5->f5=((
_tmp8F4="Always print kinds of type variables",_tag_dyneither(_tmp8F4,sizeof(
char),37))),_tmp8F5)))))))))))),((_tmp7B1[52]=((_tmp8EF=_region_malloc(r,sizeof(*
_tmp8EF)),((_tmp8EF->f1=((_tmp8EA="-printalltvars",_tag_dyneither(_tmp8EA,
sizeof(char),15))),((_tmp8EF->f2=0,((_tmp8EF->f3=((_tmp8EB="",_tag_dyneither(
_tmp8EB,sizeof(char),1))),((_tmp8EF->f4=(void*)((_tmp8EC=_region_malloc(r,
sizeof(*_tmp8EC)),((_tmp8EC[0]=((_tmp8ED.tag=3,((_tmp8ED.f1=& Cyc_print_all_tvars_r,
_tmp8ED)))),_tmp8EC)))),((_tmp8EF->f5=((_tmp8EE="Print all type variables (even implicit default effects)",
_tag_dyneither(_tmp8EE,sizeof(char),57))),_tmp8EF)))))))))))),((_tmp7B1[51]=((
_tmp8E9=_region_malloc(r,sizeof(*_tmp8E9)),((_tmp8E9->f1=((_tmp8E4="-noexpandtypedefs",
_tag_dyneither(_tmp8E4,sizeof(char),18))),((_tmp8E9->f2=0,((_tmp8E9->f3=((
_tmp8E5="",_tag_dyneither(_tmp8E5,sizeof(char),1))),((_tmp8E9->f4=(void*)((
_tmp8E6=_region_malloc(r,sizeof(*_tmp8E6)),((_tmp8E6[0]=((_tmp8E7.tag=3,((
_tmp8E7.f1=& Cyc_noexpand_r,_tmp8E7)))),_tmp8E6)))),((_tmp8E9->f5=((_tmp8E8="Don't expand typedefs in pretty printing",
_tag_dyneither(_tmp8E8,sizeof(char),41))),_tmp8E9)))))))))))),((_tmp7B1[50]=((
_tmp8E3=_region_malloc(r,sizeof(*_tmp8E3)),((_tmp8E3->f1=((_tmp8DE="-noremoveunused",
_tag_dyneither(_tmp8DE,sizeof(char),16))),((_tmp8E3->f2=0,((_tmp8E3->f3=((
_tmp8DF="",_tag_dyneither(_tmp8DF,sizeof(char),1))),((_tmp8E3->f4=(void*)((
_tmp8E0=_region_malloc(r,sizeof(*_tmp8E0)),((_tmp8E0[0]=((_tmp8E1.tag=3,((
_tmp8E1.f1=& Cyc_noshake_r,_tmp8E1)))),_tmp8E0)))),((_tmp8E3->f5=((_tmp8E2="Don't remove externed variables that aren't used",
_tag_dyneither(_tmp8E2,sizeof(char),49))),_tmp8E3)))))))))))),((_tmp7B1[49]=((
_tmp8DD=_region_malloc(r,sizeof(*_tmp8DD)),((_tmp8DD->f1=((_tmp8D8="-nogc",
_tag_dyneither(_tmp8D8,sizeof(char),6))),((_tmp8DD->f2=0,((_tmp8DD->f3=((_tmp8D9="",
_tag_dyneither(_tmp8D9,sizeof(char),1))),((_tmp8DD->f4=(void*)((_tmp8DA=
_region_malloc(r,sizeof(*_tmp8DA)),((_tmp8DA[0]=((_tmp8DB.tag=3,((_tmp8DB.f1=&
Cyc_nogc_r,_tmp8DB)))),_tmp8DA)))),((_tmp8DD->f5=((_tmp8DC="Don't link in the garbage collector",
_tag_dyneither(_tmp8DC,sizeof(char),36))),_tmp8DD)))))))))))),((_tmp7B1[48]=((
_tmp8D7=_region_malloc(r,sizeof(*_tmp8D7)),((_tmp8D7->f1=((_tmp8D2="-nocyc",
_tag_dyneither(_tmp8D2,sizeof(char),7))),((_tmp8D7->f2=0,((_tmp8D7->f3=((_tmp8D3="",
_tag_dyneither(_tmp8D3,sizeof(char),1))),((_tmp8D7->f4=(void*)((_tmp8D4=
_region_malloc(r,sizeof(*_tmp8D4)),((_tmp8D4[0]=((_tmp8D5.tag=0,((_tmp8D5.f1=Cyc_set_nocyc,
_tmp8D5)))),_tmp8D4)))),((_tmp8D7->f5=((_tmp8D6="Don't add implicit namespace Cyc",
_tag_dyneither(_tmp8D6,sizeof(char),33))),_tmp8D7)))))))))))),((_tmp7B1[47]=((
_tmp8D1=_region_malloc(r,sizeof(*_tmp8D1)),((_tmp8D1->f1=((_tmp8CC="-no-cpp-precomp",
_tag_dyneither(_tmp8CC,sizeof(char),16))),((_tmp8D1->f2=0,((_tmp8D1->f3=((
_tmp8CD="",_tag_dyneither(_tmp8CD,sizeof(char),1))),((_tmp8D1->f4=(void*)((
_tmp8CE=_region_malloc(r,sizeof(*_tmp8CE)),((_tmp8CE[0]=((_tmp8CF.tag=0,((
_tmp8CF.f1=Cyc_set_nocppprecomp,_tmp8CF)))),_tmp8CE)))),((_tmp8D1->f5=((_tmp8D0="Don't do smart preprocessing (mac only)",
_tag_dyneither(_tmp8D0,sizeof(char),40))),_tmp8D1)))))))))))),((_tmp7B1[46]=((
_tmp8CB=_region_malloc(r,sizeof(*_tmp8CB)),((_tmp8CB->f1=((_tmp8C6="-use-cpp",
_tag_dyneither(_tmp8C6,sizeof(char),9))),((_tmp8CB->f2=0,((_tmp8CB->f3=((_tmp8C7="<path>",
_tag_dyneither(_tmp8C7,sizeof(char),7))),((_tmp8CB->f4=(void*)((_tmp8C8=
_region_malloc(r,sizeof(*_tmp8C8)),((_tmp8C8[0]=((_tmp8C9.tag=5,((_tmp8C9.f1=Cyc_set_cpp,
_tmp8C9)))),_tmp8C8)))),((_tmp8CB->f5=((_tmp8CA="Indicate which preprocessor to use",
_tag_dyneither(_tmp8CA,sizeof(char),35))),_tmp8CB)))))))))))),((_tmp7B1[45]=((
_tmp8C5=_region_malloc(r,sizeof(*_tmp8C5)),((_tmp8C5->f1=((_tmp8C0="--inline-checks",
_tag_dyneither(_tmp8C0,sizeof(char),16))),((_tmp8C5->f2=0,((_tmp8C5->f3=((
_tmp8C1="",_tag_dyneither(_tmp8C1,sizeof(char),1))),((_tmp8C5->f4=(void*)((
_tmp8C2=_region_malloc(r,sizeof(*_tmp8C2)),((_tmp8C2[0]=((_tmp8C3.tag=0,((
_tmp8C3.f1=Cyc_set_inline_functions,_tmp8C3)))),_tmp8C2)))),((_tmp8C5->f5=((
_tmp8C4="Inline bounds checks instead of #define",_tag_dyneither(_tmp8C4,sizeof(
char),40))),_tmp8C5)))))))))))),((_tmp7B1[44]=((_tmp8BF=_region_malloc(r,sizeof(*
_tmp8BF)),((_tmp8BF->f1=((_tmp8BA="--noboundschecks",_tag_dyneither(_tmp8BA,
sizeof(char),17))),((_tmp8BF->f2=0,((_tmp8BF->f3=((_tmp8BB="",_tag_dyneither(
_tmp8BB,sizeof(char),1))),((_tmp8BF->f4=(void*)((_tmp8BC=_region_malloc(r,
sizeof(*_tmp8BC)),((_tmp8BC[0]=((_tmp8BD.tag=0,((_tmp8BD.f1=Cyc_set_noboundschecks,
_tmp8BD)))),_tmp8BC)))),((_tmp8BF->f5=((_tmp8BE="Disable bounds checks",
_tag_dyneither(_tmp8BE,sizeof(char),22))),_tmp8BF)))))))))))),((_tmp7B1[43]=((
_tmp8B9=_region_malloc(r,sizeof(*_tmp8B9)),((_tmp8B9->f1=((_tmp8B4="--nonullchecks",
_tag_dyneither(_tmp8B4,sizeof(char),15))),((_tmp8B9->f2=0,((_tmp8B9->f3=((
_tmp8B5="",_tag_dyneither(_tmp8B5,sizeof(char),1))),((_tmp8B9->f4=(void*)((
_tmp8B6=_region_malloc(r,sizeof(*_tmp8B6)),((_tmp8B6[0]=((_tmp8B7.tag=0,((
_tmp8B7.f1=Cyc_set_nonullchecks,_tmp8B7)))),_tmp8B6)))),((_tmp8B9->f5=((_tmp8B8="Disable null checks",
_tag_dyneither(_tmp8B8,sizeof(char),20))),_tmp8B9)))))))))))),((_tmp7B1[42]=((
_tmp8B3=_region_malloc(r,sizeof(*_tmp8B3)),((_tmp8B3->f1=((_tmp8AE="--nochecks",
_tag_dyneither(_tmp8AE,sizeof(char),11))),((_tmp8B3->f2=0,((_tmp8B3->f3=((
_tmp8AF="",_tag_dyneither(_tmp8AF,sizeof(char),1))),((_tmp8B3->f4=(void*)((
_tmp8B0=_region_malloc(r,sizeof(*_tmp8B0)),((_tmp8B0[0]=((_tmp8B1.tag=0,((
_tmp8B1.f1=Cyc_set_nochecks,_tmp8B1)))),_tmp8B0)))),((_tmp8B3->f5=((_tmp8B2="Disable bounds/null checks",
_tag_dyneither(_tmp8B2,sizeof(char),27))),_tmp8B3)))))))))))),((_tmp7B1[41]=((
_tmp8AD=_region_malloc(r,sizeof(*_tmp8AD)),((_tmp8AD->f1=((_tmp8A8="--lineno",
_tag_dyneither(_tmp8A8,sizeof(char),9))),((_tmp8AD->f2=0,((_tmp8AD->f3=((_tmp8A9="",
_tag_dyneither(_tmp8A9,sizeof(char),1))),((_tmp8AD->f4=(void*)((_tmp8AA=
_region_malloc(r,sizeof(*_tmp8AA)),((_tmp8AA[0]=((_tmp8AB.tag=0,((_tmp8AB.f1=Cyc_set_lineno,
_tmp8AB)))),_tmp8AA)))),((_tmp8AD->f5=((_tmp8AC="Generate line numbers for debugging",
_tag_dyneither(_tmp8AC,sizeof(char),36))),_tmp8AD)))))))))))),((_tmp7B1[40]=((
_tmp8A7=_region_malloc(r,sizeof(*_tmp8A7)),((_tmp8A7->f1=((_tmp8A2="-save-c",
_tag_dyneither(_tmp8A2,sizeof(char),8))),((_tmp8A7->f2=0,((_tmp8A7->f3=((_tmp8A3="",
_tag_dyneither(_tmp8A3,sizeof(char),1))),((_tmp8A7->f4=(void*)((_tmp8A4=
_region_malloc(r,sizeof(*_tmp8A4)),((_tmp8A4[0]=((_tmp8A5.tag=3,((_tmp8A5.f1=&
Cyc_save_c_r,_tmp8A5)))),_tmp8A4)))),((_tmp8A7->f5=((_tmp8A6="Don't delete temporary C files",
_tag_dyneither(_tmp8A6,sizeof(char),31))),_tmp8A7)))))))))))),((_tmp7B1[39]=((
_tmp8A1=_region_malloc(r,sizeof(*_tmp8A1)),((_tmp8A1->f1=((_tmp89C="-save-temps",
_tag_dyneither(_tmp89C,sizeof(char),12))),((_tmp8A1->f2=0,((_tmp8A1->f3=((
_tmp89D="",_tag_dyneither(_tmp89D,sizeof(char),1))),((_tmp8A1->f4=(void*)((
_tmp89E=_region_malloc(r,sizeof(*_tmp89E)),((_tmp89E[0]=((_tmp89F.tag=0,((
_tmp89F.f1=Cyc_set_save_temps,_tmp89F)))),_tmp89E)))),((_tmp8A1->f5=((_tmp8A0="Don't delete temporary files",
_tag_dyneither(_tmp8A0,sizeof(char),29))),_tmp8A1)))))))))))),((_tmp7B1[38]=((
_tmp89B=_region_malloc(r,sizeof(*_tmp89B)),((_tmp89B->f1=((_tmp896="-c-comp",
_tag_dyneither(_tmp896,sizeof(char),8))),((_tmp89B->f2=0,((_tmp89B->f3=((_tmp897=" <compiler>",
_tag_dyneither(_tmp897,sizeof(char),12))),((_tmp89B->f4=(void*)((_tmp898=
_region_malloc(r,sizeof(*_tmp898)),((_tmp898[0]=((_tmp899.tag=5,((_tmp899.f1=Cyc_set_c_compiler,
_tmp899)))),_tmp898)))),((_tmp89B->f5=((_tmp89A="Produce C output for the given compiler",
_tag_dyneither(_tmp89A,sizeof(char),40))),_tmp89B)))))))))))),((_tmp7B1[37]=((
_tmp895=_region_malloc(r,sizeof(*_tmp895)),((_tmp895->f1=((_tmp890="-un-gcc",
_tag_dyneither(_tmp890,sizeof(char),8))),((_tmp895->f2=0,((_tmp895->f3=((_tmp891="",
_tag_dyneither(_tmp891,sizeof(char),1))),((_tmp895->f4=(void*)((_tmp892=
_region_malloc(r,sizeof(*_tmp892)),((_tmp892[0]=((_tmp893.tag=0,((_tmp893.f1=Cyc_set_tovc,
_tmp893)))),_tmp892)))),((_tmp895->f5=((_tmp894="Avoid gcc extensions in C output (except for attributes)",
_tag_dyneither(_tmp894,sizeof(char),57))),_tmp895)))))))))))),((_tmp7B1[36]=((
_tmp88F=_region_malloc(r,sizeof(*_tmp88F)),((_tmp88F->f1=((_tmp88A="-elim-statement-expressions",
_tag_dyneither(_tmp88A,sizeof(char),28))),((_tmp88F->f2=0,((_tmp88F->f3=((
_tmp88B="",_tag_dyneither(_tmp88B,sizeof(char),1))),((_tmp88F->f4=(void*)((
_tmp88C=_region_malloc(r,sizeof(*_tmp88C)),((_tmp88C[0]=((_tmp88D.tag=3,((
_tmp88D.f1=& Cyc_elim_se_r,_tmp88D)))),_tmp88C)))),((_tmp88F->f5=((_tmp88E="Avoid statement expressions in C output (implies --inline-checks)",
_tag_dyneither(_tmp88E,sizeof(char),66))),_tmp88F)))))))))))),((_tmp7B1[35]=((
_tmp889=_region_malloc(r,sizeof(*_tmp889)),((_tmp889->f1=((_tmp884="-up",
_tag_dyneither(_tmp884,sizeof(char),4))),((_tmp889->f2=0,((_tmp889->f3=((_tmp885="",
_tag_dyneither(_tmp885,sizeof(char),1))),((_tmp889->f4=(void*)((_tmp886=
_region_malloc(r,sizeof(*_tmp886)),((_tmp886[0]=((_tmp887.tag=4,((_tmp887.f1=&
Cyc_pp_r,_tmp887)))),_tmp886)))),((_tmp889->f5=((_tmp888="Ugly print the C code that Cyclone generates (default)",
_tag_dyneither(_tmp888,sizeof(char),55))),_tmp889)))))))))))),((_tmp7B1[34]=((
_tmp883=_region_malloc(r,sizeof(*_tmp883)),((_tmp883->f1=((_tmp87E="-pp",
_tag_dyneither(_tmp87E,sizeof(char),4))),((_tmp883->f2=0,((_tmp883->f3=((_tmp87F="",
_tag_dyneither(_tmp87F,sizeof(char),1))),((_tmp883->f4=(void*)((_tmp880=
_region_malloc(r,sizeof(*_tmp880)),((_tmp880[0]=((_tmp881.tag=3,((_tmp881.f1=&
Cyc_pp_r,_tmp881)))),_tmp880)))),((_tmp883->f5=((_tmp882="Pretty print the C code that Cyclone generates",
_tag_dyneither(_tmp882,sizeof(char),47))),_tmp883)))))))))))),((_tmp7B1[33]=((
_tmp87D=_region_malloc(r,sizeof(*_tmp87D)),((_tmp87D->f1=((_tmp878="-ic",
_tag_dyneither(_tmp878,sizeof(char),4))),((_tmp87D->f2=0,((_tmp87D->f3=((_tmp879="",
_tag_dyneither(_tmp879,sizeof(char),1))),((_tmp87D->f4=(void*)((_tmp87A=
_region_malloc(r,sizeof(*_tmp87A)),((_tmp87A[0]=((_tmp87B.tag=3,((_tmp87B.f1=&
Cyc_ic_r,_tmp87B)))),_tmp87A)))),((_tmp87D->f5=((_tmp87C="Activate the link-checker",
_tag_dyneither(_tmp87C,sizeof(char),26))),_tmp87D)))))))))))),((_tmp7B1[32]=((
_tmp877=_region_malloc(r,sizeof(*_tmp877)),((_tmp877->f1=((_tmp872="-stopafter-toc",
_tag_dyneither(_tmp872,sizeof(char),15))),((_tmp877->f2=0,((_tmp877->f3=((
_tmp873="",_tag_dyneither(_tmp873,sizeof(char),1))),((_tmp877->f4=(void*)((
_tmp874=_region_malloc(r,sizeof(*_tmp874)),((_tmp874[0]=((_tmp875.tag=3,((
_tmp875.f1=& Cyc_toc_r,_tmp875)))),_tmp874)))),((_tmp877->f5=((_tmp876="Stop after translation to C",
_tag_dyneither(_tmp876,sizeof(char),28))),_tmp877)))))))))))),((_tmp7B1[31]=((
_tmp871=_region_malloc(r,sizeof(*_tmp871)),((_tmp871->f1=((_tmp86C="-stopafter-cf",
_tag_dyneither(_tmp86C,sizeof(char),14))),((_tmp871->f2=0,((_tmp871->f3=((
_tmp86D="",_tag_dyneither(_tmp86D,sizeof(char),1))),((_tmp871->f4=(void*)((
_tmp86E=_region_malloc(r,sizeof(*_tmp86E)),((_tmp86E[0]=((_tmp86F.tag=3,((
_tmp86F.f1=& Cyc_cf_r,_tmp86F)))),_tmp86E)))),((_tmp871->f5=((_tmp870="Stop after control-flow checking",
_tag_dyneither(_tmp870,sizeof(char),33))),_tmp871)))))))))))),((_tmp7B1[30]=((
_tmp86B=_region_malloc(r,sizeof(*_tmp86B)),((_tmp86B->f1=((_tmp866="-noprint",
_tag_dyneither(_tmp866,sizeof(char),9))),((_tmp86B->f2=0,((_tmp86B->f3=((_tmp867="",
_tag_dyneither(_tmp867,sizeof(char),1))),((_tmp86B->f4=(void*)((_tmp868=
_region_malloc(r,sizeof(*_tmp868)),((_tmp868[0]=((_tmp869.tag=3,((_tmp869.f1=&
Cyc_noprint_r,_tmp869)))),_tmp868)))),((_tmp86B->f5=((_tmp86A="Do not print output (when stopping early)",
_tag_dyneither(_tmp86A,sizeof(char),42))),_tmp86B)))))))))))),((_tmp7B1[29]=((
_tmp865=_region_malloc(r,sizeof(*_tmp865)),((_tmp865->f1=((_tmp860="-stopafter-tc",
_tag_dyneither(_tmp860,sizeof(char),14))),((_tmp865->f2=0,((_tmp865->f3=((
_tmp861="",_tag_dyneither(_tmp861,sizeof(char),1))),((_tmp865->f4=(void*)((
_tmp862=_region_malloc(r,sizeof(*_tmp862)),((_tmp862[0]=((_tmp863.tag=3,((
_tmp863.f1=& Cyc_tc_r,_tmp863)))),_tmp862)))),((_tmp865->f5=((_tmp864="Stop after type checking",
_tag_dyneither(_tmp864,sizeof(char),25))),_tmp865)))))))))))),((_tmp7B1[28]=((
_tmp85F=_region_malloc(r,sizeof(*_tmp85F)),((_tmp85F->f1=((_tmp85A="-stopafter-parse",
_tag_dyneither(_tmp85A,sizeof(char),17))),((_tmp85F->f2=0,((_tmp85F->f3=((
_tmp85B="",_tag_dyneither(_tmp85B,sizeof(char),1))),((_tmp85F->f4=(void*)((
_tmp85C=_region_malloc(r,sizeof(*_tmp85C)),((_tmp85C[0]=((_tmp85D.tag=3,((
_tmp85D.f1=& Cyc_parseonly_r,_tmp85D)))),_tmp85C)))),((_tmp85F->f5=((_tmp85E="Stop after parsing",
_tag_dyneither(_tmp85E,sizeof(char),19))),_tmp85F)))))))))))),((_tmp7B1[27]=((
_tmp859=_region_malloc(r,sizeof(*_tmp859)),((_tmp859->f1=((_tmp854="-E",
_tag_dyneither(_tmp854,sizeof(char),3))),((_tmp859->f2=0,((_tmp859->f3=((_tmp855="",
_tag_dyneither(_tmp855,sizeof(char),1))),((_tmp859->f4=(void*)((_tmp856=
_region_malloc(r,sizeof(*_tmp856)),((_tmp856[0]=((_tmp857.tag=3,((_tmp857.f1=&
Cyc_stop_after_cpp_r,_tmp857)))),_tmp856)))),((_tmp859->f5=((_tmp858="Stop after preprocessing",
_tag_dyneither(_tmp858,sizeof(char),25))),_tmp859)))))))))))),((_tmp7B1[26]=((
_tmp853=_region_malloc(r,sizeof(*_tmp853)),((_tmp853->f1=((_tmp84E="-Wall",
_tag_dyneither(_tmp84E,sizeof(char),6))),((_tmp853->f2=0,((_tmp853->f3=((_tmp84F="",
_tag_dyneither(_tmp84F,sizeof(char),1))),((_tmp853->f4=(void*)((_tmp850=
_region_malloc(r,sizeof(*_tmp850)),((_tmp850[0]=((_tmp851.tag=0,((_tmp851.f1=Cyc_set_all_warnings,
_tmp851)))),_tmp850)))),((_tmp853->f5=((_tmp852="Turn on all warnings",
_tag_dyneither(_tmp852,sizeof(char),21))),_tmp853)))))))))))),((_tmp7B1[25]=((
_tmp84D=_region_malloc(r,sizeof(*_tmp84D)),((_tmp84D->f1=((_tmp848="-b",
_tag_dyneither(_tmp848,sizeof(char),3))),((_tmp84D->f2=0,((_tmp84D->f3=((_tmp849="<arch>",
_tag_dyneither(_tmp849,sizeof(char),7))),((_tmp84D->f4=(void*)((_tmp84A=
_region_malloc(r,sizeof(*_tmp84A)),((_tmp84A[0]=((_tmp84B.tag=5,((_tmp84B.f1=Cyc_set_target_arch,
_tmp84B)))),_tmp84A)))),((_tmp84D->f5=((_tmp84C="Set target architecture; implies -stopafter-toc",
_tag_dyneither(_tmp84C,sizeof(char),48))),_tmp84D)))))))))))),((_tmp7B1[24]=((
_tmp847=_region_malloc(r,sizeof(*_tmp847)),((_tmp847->f1=((_tmp842="-MT",
_tag_dyneither(_tmp842,sizeof(char),4))),((_tmp847->f2=0,((_tmp847->f3=((_tmp843=" <target>",
_tag_dyneither(_tmp843,sizeof(char),10))),((_tmp847->f4=(void*)((_tmp844=
_region_malloc(r,sizeof(*_tmp844)),((_tmp844[0]=((_tmp845.tag=5,((_tmp845.f1=Cyc_set_dependencies_target,
_tmp845)))),_tmp844)))),((_tmp847->f5=((_tmp846="Give target for dependencies",
_tag_dyneither(_tmp846,sizeof(char),29))),_tmp847)))))))))))),((_tmp7B1[23]=((
_tmp841=_region_malloc(r,sizeof(*_tmp841)),((_tmp841->f1=((_tmp83C="-MG",
_tag_dyneither(_tmp83C,sizeof(char),4))),((_tmp841->f2=0,((_tmp841->f3=((_tmp83D="",
_tag_dyneither(_tmp83D,sizeof(char),1))),((_tmp841->f4=(void*)((_tmp83E=
_region_malloc(r,sizeof(*_tmp83E)),((_tmp83E[0]=((_tmp83F.tag=1,((_tmp83F.f1=Cyc_add_cpparg,
_tmp83F)))),_tmp83E)))),((_tmp841->f5=((_tmp840="When producing dependencies assume that missing files are generated",
_tag_dyneither(_tmp840,sizeof(char),68))),_tmp841)))))))))))),((_tmp7B1[22]=((
_tmp83B=_region_malloc(r,sizeof(*_tmp83B)),((_tmp83B->f1=((_tmp836="-M",
_tag_dyneither(_tmp836,sizeof(char),3))),((_tmp83B->f2=0,((_tmp83B->f3=((_tmp837="",
_tag_dyneither(_tmp837,sizeof(char),1))),((_tmp83B->f4=(void*)((_tmp838=
_region_malloc(r,sizeof(*_tmp838)),((_tmp838[0]=((_tmp839.tag=0,((_tmp839.f1=Cyc_set_produce_dependencies,
_tmp839)))),_tmp838)))),((_tmp83B->f5=((_tmp83A="Produce dependencies",
_tag_dyneither(_tmp83A,sizeof(char),21))),_tmp83B)))))))))))),((_tmp7B1[21]=((
_tmp835=_region_malloc(r,sizeof(*_tmp835)),((_tmp835->f1=((_tmp830="-S",
_tag_dyneither(_tmp830,sizeof(char),3))),((_tmp835->f2=0,((_tmp835->f3=((_tmp831="",
_tag_dyneither(_tmp831,sizeof(char),1))),((_tmp835->f4=(void*)((_tmp832=
_region_malloc(r,sizeof(*_tmp832)),((_tmp832[0]=((_tmp833.tag=0,((_tmp833.f1=Cyc_set_stop_after_asmfile,
_tmp833)))),_tmp832)))),((_tmp835->f5=((_tmp834="Stop after producing assembly code",
_tag_dyneither(_tmp834,sizeof(char),35))),_tmp835)))))))))))),((_tmp7B1[20]=((
_tmp82F=_region_malloc(r,sizeof(*_tmp82F)),((_tmp82F->f1=((_tmp82A="-pa",
_tag_dyneither(_tmp82A,sizeof(char),4))),((_tmp82F->f2=0,((_tmp82F->f3=((_tmp82B="",
_tag_dyneither(_tmp82B,sizeof(char),1))),((_tmp82F->f4=(void*)((_tmp82C=
_region_malloc(r,sizeof(*_tmp82C)),((_tmp82C[0]=((_tmp82D.tag=0,((_tmp82D.f1=Cyc_set_pa,
_tmp82D)))),_tmp82C)))),((_tmp82F->f5=((_tmp82E="Compile for profiling with aprof",
_tag_dyneither(_tmp82E,sizeof(char),33))),_tmp82F)))))))))))),((_tmp7B1[19]=((
_tmp829=_region_malloc(r,sizeof(*_tmp829)),((_tmp829->f1=((_tmp824="-pg",
_tag_dyneither(_tmp824,sizeof(char),4))),((_tmp829->f2=0,((_tmp829->f3=((_tmp825="",
_tag_dyneither(_tmp825,sizeof(char),1))),((_tmp829->f4=(void*)((_tmp826=
_region_malloc(r,sizeof(*_tmp826)),((_tmp826[0]=((_tmp827.tag=0,((_tmp827.f1=Cyc_set_pg,
_tmp827)))),_tmp826)))),((_tmp829->f5=((_tmp828="Compile for profiling with gprof",
_tag_dyneither(_tmp828,sizeof(char),33))),_tmp829)))))))))))),((_tmp7B1[18]=((
_tmp823=_region_malloc(r,sizeof(*_tmp823)),((_tmp823->f1=((_tmp81E="-p",
_tag_dyneither(_tmp81E,sizeof(char),3))),((_tmp823->f2=0,((_tmp823->f3=((_tmp81F="",
_tag_dyneither(_tmp81F,sizeof(char),1))),((_tmp823->f4=(void*)((_tmp820=
_region_malloc(r,sizeof(*_tmp820)),((_tmp820[0]=((_tmp821.tag=1,((_tmp821.f1=Cyc_add_ccarg,
_tmp821)))),_tmp820)))),((_tmp823->f5=((_tmp822="Compile for profiling with prof",
_tag_dyneither(_tmp822,sizeof(char),32))),_tmp823)))))))))))),((_tmp7B1[17]=((
_tmp81D=_region_malloc(r,sizeof(*_tmp81D)),((_tmp81D->f1=((_tmp818="-g",
_tag_dyneither(_tmp818,sizeof(char),3))),((_tmp81D->f2=0,((_tmp81D->f3=((_tmp819="",
_tag_dyneither(_tmp819,sizeof(char),1))),((_tmp81D->f4=(void*)((_tmp81A=
_region_malloc(r,sizeof(*_tmp81A)),((_tmp81A[0]=((_tmp81B.tag=1,((_tmp81B.f1=Cyc_add_ccarg,
_tmp81B)))),_tmp81A)))),((_tmp81D->f5=((_tmp81C="Compile for debugging",
_tag_dyneither(_tmp81C,sizeof(char),22))),_tmp81D)))))))))))),((_tmp7B1[16]=((
_tmp817=_region_malloc(r,sizeof(*_tmp817)),((_tmp817->f1=((_tmp812="-fomit-frame-pointer",
_tag_dyneither(_tmp812,sizeof(char),21))),((_tmp817->f2=0,((_tmp817->f3=((
_tmp813="",_tag_dyneither(_tmp813,sizeof(char),1))),((_tmp817->f4=(void*)((
_tmp814=_region_malloc(r,sizeof(*_tmp814)),((_tmp814[0]=((_tmp815.tag=1,((
_tmp815.f1=Cyc_add_ccarg,_tmp815)))),_tmp814)))),((_tmp817->f5=((_tmp816="Omit frame pointer",
_tag_dyneither(_tmp816,sizeof(char),19))),_tmp817)))))))))))),((_tmp7B1[15]=((
_tmp811=_region_malloc(r,sizeof(*_tmp811)),((_tmp811->f1=((_tmp80C="-O3",
_tag_dyneither(_tmp80C,sizeof(char),4))),((_tmp811->f2=0,((_tmp811->f3=((_tmp80D="",
_tag_dyneither(_tmp80D,sizeof(char),1))),((_tmp811->f4=(void*)((_tmp80E=
_region_malloc(r,sizeof(*_tmp80E)),((_tmp80E[0]=((_tmp80F.tag=1,((_tmp80F.f1=Cyc_add_ccarg,
_tmp80F)))),_tmp80E)))),((_tmp811->f5=((_tmp810="Even more optimization",
_tag_dyneither(_tmp810,sizeof(char),23))),_tmp811)))))))))))),((_tmp7B1[14]=((
_tmp80B=_region_malloc(r,sizeof(*_tmp80B)),((_tmp80B->f1=((_tmp806="-O2",
_tag_dyneither(_tmp806,sizeof(char),4))),((_tmp80B->f2=0,((_tmp80B->f3=((_tmp807="",
_tag_dyneither(_tmp807,sizeof(char),1))),((_tmp80B->f4=(void*)((_tmp808=
_region_malloc(r,sizeof(*_tmp808)),((_tmp808[0]=((_tmp809.tag=1,((_tmp809.f1=Cyc_add_ccarg,
_tmp809)))),_tmp808)))),((_tmp80B->f5=((_tmp80A="A higher level of optimization",
_tag_dyneither(_tmp80A,sizeof(char),31))),_tmp80B)))))))))))),((_tmp7B1[13]=((
_tmp805=_region_malloc(r,sizeof(*_tmp805)),((_tmp805->f1=((_tmp800="-O",
_tag_dyneither(_tmp800,sizeof(char),3))),((_tmp805->f2=0,((_tmp805->f3=((_tmp801="",
_tag_dyneither(_tmp801,sizeof(char),1))),((_tmp805->f4=(void*)((_tmp802=
_region_malloc(r,sizeof(*_tmp802)),((_tmp802[0]=((_tmp803.tag=1,((_tmp803.f1=Cyc_add_ccarg,
_tmp803)))),_tmp802)))),((_tmp805->f5=((_tmp804="Optimize",_tag_dyneither(
_tmp804,sizeof(char),9))),_tmp805)))))))))))),((_tmp7B1[12]=((_tmp7FF=
_region_malloc(r,sizeof(*_tmp7FF)),((_tmp7FF->f1=((_tmp7FA="-O0",_tag_dyneither(
_tmp7FA,sizeof(char),4))),((_tmp7FF->f2=0,((_tmp7FF->f3=((_tmp7FB="",
_tag_dyneither(_tmp7FB,sizeof(char),1))),((_tmp7FF->f4=(void*)((_tmp7FC=
_region_malloc(r,sizeof(*_tmp7FC)),((_tmp7FC[0]=((_tmp7FD.tag=1,((_tmp7FD.f1=Cyc_add_ccarg,
_tmp7FD)))),_tmp7FC)))),((_tmp7FF->f5=((_tmp7FE="Don't optimize",_tag_dyneither(
_tmp7FE,sizeof(char),15))),_tmp7FF)))))))))))),((_tmp7B1[11]=((_tmp7F9=
_region_malloc(r,sizeof(*_tmp7F9)),((_tmp7F9->f1=((_tmp7F4="-s",_tag_dyneither(
_tmp7F4,sizeof(char),3))),((_tmp7F9->f2=0,((_tmp7F9->f3=((_tmp7F5="",
_tag_dyneither(_tmp7F5,sizeof(char),1))),((_tmp7F9->f4=(void*)((_tmp7F6=
_region_malloc(r,sizeof(*_tmp7F6)),((_tmp7F6[0]=((_tmp7F7.tag=1,((_tmp7F7.f1=Cyc_add_ccarg,
_tmp7F7)))),_tmp7F6)))),((_tmp7F9->f5=((_tmp7F8="Remove all symbol table and relocation info from executable",
_tag_dyneither(_tmp7F8,sizeof(char),60))),_tmp7F9)))))))))))),((_tmp7B1[10]=((
_tmp7F3=_region_malloc(r,sizeof(*_tmp7F3)),((_tmp7F3->f1=((_tmp7EE="-x",
_tag_dyneither(_tmp7EE,sizeof(char),3))),((_tmp7F3->f2=0,((_tmp7F3->f3=((_tmp7EF=" <language>",
_tag_dyneither(_tmp7EF,sizeof(char),12))),((_tmp7F3->f4=(void*)((_tmp7F0=
_region_malloc(r,sizeof(*_tmp7F0)),((_tmp7F0[0]=((_tmp7F1.tag=5,((_tmp7F1.f1=Cyc_set_inputtype,
_tmp7F1)))),_tmp7F0)))),((_tmp7F3->f5=((_tmp7F2="Specify <language> for the following input files",
_tag_dyneither(_tmp7F2,sizeof(char),49))),_tmp7F3)))))))))))),((_tmp7B1[9]=((
_tmp7ED=_region_malloc(r,sizeof(*_tmp7ED)),((_tmp7ED->f1=((_tmp7E8="-c",
_tag_dyneither(_tmp7E8,sizeof(char),3))),((_tmp7ED->f2=0,((_tmp7ED->f3=((_tmp7E9="",
_tag_dyneither(_tmp7E9,sizeof(char),1))),((_tmp7ED->f4=(void*)((_tmp7EA=
_region_malloc(r,sizeof(*_tmp7EA)),((_tmp7EA[0]=((_tmp7EB.tag=0,((_tmp7EB.f1=Cyc_set_stop_after_objectfile,
_tmp7EB)))),_tmp7EA)))),((_tmp7ED->f5=((_tmp7EC="Produce an object file instead of an executable; do not link",
_tag_dyneither(_tmp7EC,sizeof(char),61))),_tmp7ED)))))))))))),((_tmp7B1[8]=((
_tmp7E7=_region_malloc(r,sizeof(*_tmp7E7)),((_tmp7E7->f1=((_tmp7E2="-m",
_tag_dyneither(_tmp7E2,sizeof(char),3))),((_tmp7E7->f2=1,((_tmp7E7->f3=((_tmp7E3="<option>",
_tag_dyneither(_tmp7E3,sizeof(char),9))),((_tmp7E7->f4=(void*)((_tmp7E4=
_region_malloc(r,sizeof(*_tmp7E4)),((_tmp7E4[0]=((_tmp7E5.tag=1,((_tmp7E5.f1=Cyc_add_marg,
_tmp7E5)))),_tmp7E4)))),((_tmp7E7->f5=((_tmp7E6="GCC specific: pass machine-dependent flag on to gcc",
_tag_dyneither(_tmp7E6,sizeof(char),52))),_tmp7E7)))))))))))),((_tmp7B1[7]=((
_tmp7E1=_region_malloc(r,sizeof(*_tmp7E1)),((_tmp7E1->f1=((_tmp7DC="-l",
_tag_dyneither(_tmp7DC,sizeof(char),3))),((_tmp7E1->f2=1,((_tmp7E1->f3=((_tmp7DD="<libname>",
_tag_dyneither(_tmp7DD,sizeof(char),10))),((_tmp7E1->f4=(void*)((_tmp7DE=
_region_malloc(r,sizeof(*_tmp7DE)),((_tmp7DE[0]=((_tmp7DF.tag=1,((_tmp7DF.f1=Cyc_add_libarg,
_tmp7DF)))),_tmp7DE)))),((_tmp7E1->f5=((_tmp7E0="Library file",_tag_dyneither(
_tmp7E0,sizeof(char),13))),_tmp7E1)))))))))))),((_tmp7B1[6]=((_tmp7DB=
_region_malloc(r,sizeof(*_tmp7DB)),((_tmp7DB->f1=((_tmp7D6="-L",_tag_dyneither(
_tmp7D6,sizeof(char),3))),((_tmp7DB->f2=1,((_tmp7DB->f3=((_tmp7D7="<dir>",
_tag_dyneither(_tmp7D7,sizeof(char),6))),((_tmp7DB->f4=(void*)((_tmp7D8=
_region_malloc(r,sizeof(*_tmp7D8)),((_tmp7D8[0]=((_tmp7D9.tag=1,((_tmp7D9.f1=Cyc_add_ccarg,
_tmp7D9)))),_tmp7D8)))),((_tmp7DB->f5=((_tmp7DA="Add to the list of directories for -l",
_tag_dyneither(_tmp7DA,sizeof(char),38))),_tmp7DB)))))))))))),((_tmp7B1[5]=((
_tmp7D5=_region_malloc(r,sizeof(*_tmp7D5)),((_tmp7D5->f1=((_tmp7D0="-I",
_tag_dyneither(_tmp7D0,sizeof(char),3))),((_tmp7D5->f2=1,((_tmp7D5->f3=((_tmp7D1="<dir>",
_tag_dyneither(_tmp7D1,sizeof(char),6))),((_tmp7D5->f4=(void*)((_tmp7D2=
_region_malloc(r,sizeof(*_tmp7D2)),((_tmp7D2[0]=((_tmp7D3.tag=1,((_tmp7D3.f1=Cyc_add_cpparg,
_tmp7D3)))),_tmp7D2)))),((_tmp7D5->f5=((_tmp7D4="Add to the list of directories to search for include files",
_tag_dyneither(_tmp7D4,sizeof(char),59))),_tmp7D5)))))))))))),((_tmp7B1[4]=((
_tmp7CF=_region_malloc(r,sizeof(*_tmp7CF)),((_tmp7CF->f1=((_tmp7CA="-B",
_tag_dyneither(_tmp7CA,sizeof(char),3))),((_tmp7CF->f2=1,((_tmp7CF->f3=((_tmp7CB="<file>",
_tag_dyneither(_tmp7CB,sizeof(char),7))),((_tmp7CF->f4=(void*)((_tmp7CC=
_region_malloc(r,sizeof(*_tmp7CC)),((_tmp7CC[0]=((_tmp7CD.tag=1,((_tmp7CD.f1=Cyc_add_cyclone_exec_path,
_tmp7CD)))),_tmp7CC)))),((_tmp7CF->f5=((_tmp7CE="Add to the list of directories to search for compiler files",
_tag_dyneither(_tmp7CE,sizeof(char),60))),_tmp7CF)))))))))))),((_tmp7B1[3]=((
_tmp7C9=_region_malloc(r,sizeof(*_tmp7C9)),((_tmp7C9->f1=((_tmp7C4="-D",
_tag_dyneither(_tmp7C4,sizeof(char),3))),((_tmp7C9->f2=1,((_tmp7C9->f3=((_tmp7C5="<name>[=<value>]",
_tag_dyneither(_tmp7C5,sizeof(char),17))),((_tmp7C9->f4=(void*)((_tmp7C6=
_region_malloc(r,sizeof(*_tmp7C6)),((_tmp7C6[0]=((_tmp7C7.tag=1,((_tmp7C7.f1=Cyc_add_cpparg,
_tmp7C7)))),_tmp7C6)))),((_tmp7C9->f5=((_tmp7C8="Pass definition to preprocessor",
_tag_dyneither(_tmp7C8,sizeof(char),32))),_tmp7C9)))))))))))),((_tmp7B1[2]=((
_tmp7C3=_region_malloc(r,sizeof(*_tmp7C3)),((_tmp7C3->f1=((_tmp7BE="-o",
_tag_dyneither(_tmp7BE,sizeof(char),3))),((_tmp7C3->f2=0,((_tmp7C3->f3=((_tmp7BF=" <file>",
_tag_dyneither(_tmp7BF,sizeof(char),8))),((_tmp7C3->f4=(void*)((_tmp7C0=
_region_malloc(r,sizeof(*_tmp7C0)),((_tmp7C0[0]=((_tmp7C1.tag=5,((_tmp7C1.f1=Cyc_set_output_file,
_tmp7C1)))),_tmp7C0)))),((_tmp7C3->f5=((_tmp7C2="Set the output file name to <file>",
_tag_dyneither(_tmp7C2,sizeof(char),35))),_tmp7C3)))))))))))),((_tmp7B1[1]=((
_tmp7BD=_region_malloc(r,sizeof(*_tmp7BD)),((_tmp7BD->f1=((_tmp7B8="--version",
_tag_dyneither(_tmp7B8,sizeof(char),10))),((_tmp7BD->f2=0,((_tmp7BD->f3=((
_tmp7B9="",_tag_dyneither(_tmp7B9,sizeof(char),1))),((_tmp7BD->f4=(void*)((
_tmp7BA=_region_malloc(r,sizeof(*_tmp7BA)),((_tmp7BA[0]=((_tmp7BB.tag=0,((
_tmp7BB.f1=Cyc_print_version,_tmp7BB)))),_tmp7BA)))),((_tmp7BD->f5=((_tmp7BC="Print version information and exit",
_tag_dyneither(_tmp7BC,sizeof(char),35))),_tmp7BD)))))))))))),((_tmp7B1[0]=((
_tmp7B7=_region_malloc(r,sizeof(*_tmp7B7)),((_tmp7B7->f1=((_tmp7B2="-v",
_tag_dyneither(_tmp7B2,sizeof(char),3))),((_tmp7B7->f2=0,((_tmp7B7->f3=((_tmp7B3="",
_tag_dyneither(_tmp7B3,sizeof(char),1))),((_tmp7B7->f4=(void*)((_tmp7B4=
_region_malloc(r,sizeof(*_tmp7B4)),((_tmp7B4[0]=((_tmp7B5.tag=3,((_tmp7B5.f1=&
Cyc_v_r,_tmp7B5)))),_tmp7B4)))),((_tmp7B7->f5=((_tmp7B6="Print compilation stages verbosely",
_tag_dyneither(_tmp7B6,sizeof(char),35))),_tmp7B7)))))))))))),((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r,_tag_dyneither(
_tmp7B1,sizeof(struct _tuple22*),64)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))));
const char*_tmp932;struct _dyneither_ptr optstring=(_tmp932="Options:",
_tag_dyneither(_tmp932,sizeof(char),9));Cyc_Arg_current=0;Cyc_Arg_parse(options,
Cyc_add_other,optstring,_tmp153);if(Cyc_target_arch.curr == (_tag_dyneither(0,0,0)).curr)
Cyc_target_arch=(struct _dyneither_ptr)Cstring_to_string(Carch);{char*_tmp933;
struct _dyneither_ptr cyclone_exec_prefix=(_tmp933=getenv((const char*)"CYCLONE_EXEC_PREFIX"),
_tag_dyneither(_tmp933,sizeof(char),_get_zero_arr_size_char(_tmp933,1)));if(
cyclone_exec_prefix.curr != (_tag_dyneither(0,0,0)).curr){struct _dyneither_ptr*
_tmp936;struct Cyc_List_List*_tmp935;Cyc_cyclone_exec_path=((_tmp935=_cycalloc(
sizeof(*_tmp935)),((_tmp935->hd=((_tmp936=_cycalloc(sizeof(*_tmp936)),((_tmp936[
0]=cyclone_exec_prefix,_tmp936)))),((_tmp935->tl=Cyc_cyclone_exec_path,_tmp935))))));}
def_lib_path=(struct _dyneither_ptr)Cstring_to_string(Cdef_lib_path);if(Cyc_strlen((
struct _dyneither_ptr)def_lib_path)> 0){struct _dyneither_ptr*_tmp93C;const char*
_tmp93B;struct Cyc_List_List*_tmp93A;Cyc_cyclone_exec_path=((_tmp93A=_cycalloc(
sizeof(*_tmp93A)),((_tmp93A->hd=((_tmp93C=_cycalloc(sizeof(*_tmp93C)),((_tmp93C[
0]=(struct _dyneither_ptr)Cyc_Filename_concat(def_lib_path,((_tmp93B="cyc-lib",
_tag_dyneither(_tmp93B,sizeof(char),8)))),_tmp93C)))),((_tmp93A->tl=Cyc_cyclone_exec_path,
_tmp93A))))));}Cyc_cyclone_exec_path=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_cyclone_exec_path);cyclone_arch_path=Cyc_also_subdir(
Cyc_cyclone_exec_path,Cyc_target_arch);{const char*_tmp93D;struct _dyneither_ptr
_tmp15A=Cyc_do_find(cyclone_arch_path,((_tmp93D="cycspecs",_tag_dyneither(
_tmp93D,sizeof(char),9))));if(Cyc_v_r){const char*_tmp941;void*_tmp940[1];struct
Cyc_String_pa_struct _tmp93F;(_tmp93F.tag=0,((_tmp93F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp15A),((_tmp940[0]=& _tmp93F,Cyc_fprintf(Cyc_stderr,((
_tmp941="Reading from specs file %s\n",_tag_dyneither(_tmp941,sizeof(char),28))),
_tag_dyneither(_tmp940,sizeof(void*),1)))))));}{struct Cyc_List_List*_tmp15E=Cyc_read_specs(
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
0;Cyc_Arg_parse(options,Cyc_add_other,optstring,_tmp154);{const char*_tmp942;if(
Cyc_strcmp((struct _dyneither_ptr)Cyc_cpp,((_tmp942="",_tag_dyneither(_tmp942,
sizeof(char),1))))== 0){const char*_tmp169=Cyc_produce_dependencies?"":(const char*)" -E";
const char*_tmp94A;const char*_tmp949;void*_tmp948[3];struct Cyc_String_pa_struct
_tmp947;struct Cyc_String_pa_struct _tmp946;struct Cyc_String_pa_struct _tmp945;Cyc_set_cpp((
struct _dyneither_ptr)((_tmp945.tag=0,((_tmp945.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp15A),((_tmp946.tag=0,((_tmp946.f1=(struct _dyneither_ptr)((
_tmp94A=_tmp169,_tag_dyneither(_tmp94A,sizeof(char),_get_zero_arr_size_char(
_tmp94A,1)))),((_tmp947.tag=0,((_tmp947.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)comp),((_tmp948[0]=& _tmp947,((_tmp948[1]=& _tmp946,((_tmp948[2]=&
_tmp945,Cyc_aprintf(((_tmp949="%s -w -x c%s -specs %s",_tag_dyneither(_tmp949,
sizeof(char),23))),_tag_dyneither(_tmp948,sizeof(void*),3)))))))))))))))))))));}};};};};};};}
if(Cyc_cyclone_files == 0  && Cyc_ccargs == 0){{const char*_tmp94D;void*_tmp94C;(
_tmp94C=0,Cyc_fprintf(Cyc_stderr,((_tmp94D="missing file\n",_tag_dyneither(
_tmp94D,sizeof(char),14))),_tag_dyneither(_tmp94C,sizeof(void*),0)));}exit(1);}
if(!Cyc_stop_after_cpp_r  && !Cyc_nocyc_setjmp_r){const char*_tmp94E;Cyc_cyc_setjmp=
Cyc_do_find(cyclone_arch_path,((_tmp94E="cyc_setjmp.h",_tag_dyneither(_tmp94E,
sizeof(char),13))));}if(!Cyc_stop_after_cpp_r  && Cyc_strlen((struct
_dyneither_ptr)Cyc_cyc_include)== 0){const char*_tmp94F;Cyc_cyc_include=Cyc_do_find(
Cyc_cyclone_exec_path,((_tmp94F="cyc_include.h",_tag_dyneither(_tmp94F,sizeof(
char),14))));}{struct Cyc_List_List*_tmp304=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(Cyc_cyclone_files);for(0;_tmp304 != 0;_tmp304=_tmp304->tl){Cyc_process_file(*((
struct _dyneither_ptr*)_tmp304->hd));if(Cyc_compile_failure){int _tmp305=1;
_npop_handler(0);return _tmp305;}}}if(((Cyc_stop_after_cpp_r  || Cyc_parseonly_r)
 || Cyc_tc_r) || Cyc_toc_r){int _tmp306=0;_npop_handler(0);return _tmp306;}if(Cyc_ccargs
== 0){int _tmp307=0;_npop_handler(0);return _tmp307;}{const char*_tmp950;Cyc_add_ccarg((
struct _dyneither_ptr)Cyc_strconcat(((_tmp950="-L",_tag_dyneither(_tmp950,sizeof(
char),3))),(struct _dyneither_ptr)def_lib_path));}Cyc_ccargs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{const char*_tmp951;struct
_dyneither_ptr _tmp309=Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct
_dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,Cyc_ccargs),((_tmp951=" ",_tag_dyneither(_tmp951,sizeof(
char),2))));Cyc_libargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_libargs);{const char*_tmp953;struct Cyc_List_List*_tmp952;struct _dyneither_ptr
_tmp30A=Cyc_str_sepstr(((_tmp952=_cycalloc(sizeof(*_tmp952)),((_tmp952->hd=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),((
_tmp952->tl=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,
Cyc_libargs),_tmp952)))))),((_tmp953=" ",_tag_dyneither(_tmp953,sizeof(char),2))));
struct Cyc_List_List*stdlib;struct _dyneither_ptr stdlib_string;const char*_tmp955;
const char*_tmp954;int _tmp30B=((Cyc_stop_after_asmfile_r  || Cyc_stop_after_objectfile_r)
 || Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)
_check_null(Cyc_output_file)),((_tmp955=".a",_tag_dyneither(_tmp955,sizeof(char),
3))))) || Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct
_dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp954=".lib",_tag_dyneither(
_tmp954,sizeof(char),5))));if(_tmp30B){stdlib=0;{const char*_tmp956;stdlib_string=(
struct _dyneither_ptr)((_tmp956="",_tag_dyneither(_tmp956,sizeof(char),1)));};}
else{struct _dyneither_ptr libcyc_flag;struct _dyneither_ptr nogc_filename;struct
_dyneither_ptr runtime_filename;struct _dyneither_ptr gc_filename;if(Cyc_pa_r){{
const char*_tmp957;libcyc_flag=((_tmp957="-lcyc_a",_tag_dyneither(_tmp957,sizeof(
char),8)));}{const char*_tmp958;nogc_filename=((_tmp958="nogc_a.a",_tag_dyneither(
_tmp958,sizeof(char),9)));}{const char*_tmp959;runtime_filename=((_tmp959="runtime_cyc_a.o",
_tag_dyneither(_tmp959,sizeof(char),16)));};}else{if(Cyc_nocheck_r){{const char*
_tmp95A;libcyc_flag=((_tmp95A="-lcyc_nocheck",_tag_dyneither(_tmp95A,sizeof(char),
14)));}{const char*_tmp95B;nogc_filename=((_tmp95B="nogc_nocheck.a",
_tag_dyneither(_tmp95B,sizeof(char),15)));}{const char*_tmp95C;runtime_filename=((
_tmp95C="runtime_cyc.o",_tag_dyneither(_tmp95C,sizeof(char),14)));};}else{if(Cyc_pg_r){{
const char*_tmp95D;libcyc_flag=((_tmp95D="-lcyc_pg",_tag_dyneither(_tmp95D,
sizeof(char),9)));}{const char*_tmp95E;runtime_filename=((_tmp95E="runtime_cyc_pg.o",
_tag_dyneither(_tmp95E,sizeof(char),17)));}{const char*_tmp95F;nogc_filename=((
_tmp95F="nogc_pg.a",_tag_dyneither(_tmp95F,sizeof(char),10)));};}else{{const char*
_tmp960;libcyc_flag=((_tmp960="-lcyc",_tag_dyneither(_tmp960,sizeof(char),6)));}{
const char*_tmp961;nogc_filename=((_tmp961="nogc.a",_tag_dyneither(_tmp961,
sizeof(char),7)));}{const char*_tmp962;runtime_filename=((_tmp962="runtime_cyc.o",
_tag_dyneither(_tmp962,sizeof(char),14)));};}}}{const char*_tmp963;gc_filename=((
_tmp963="gc.a",_tag_dyneither(_tmp963,sizeof(char),5)));}{struct _dyneither_ptr
_tmp31A=Cyc_nogc_r?Cyc_do_find(cyclone_arch_path,nogc_filename): Cyc_do_find(
cyclone_arch_path,gc_filename);struct _dyneither_ptr _tmp31B=Cyc_do_find(
cyclone_arch_path,runtime_filename);stdlib=0;{const char*_tmp969;void*_tmp968[3];
struct Cyc_String_pa_struct _tmp967;struct Cyc_String_pa_struct _tmp966;struct Cyc_String_pa_struct
_tmp965;stdlib_string=(struct _dyneither_ptr)((_tmp965.tag=0,((_tmp965.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp31A),((_tmp966.tag=0,((_tmp966.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)libcyc_flag),((_tmp967.tag=0,((
_tmp967.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp31B),((_tmp968[0]=&
_tmp967,((_tmp968[1]=& _tmp966,((_tmp968[2]=& _tmp965,Cyc_aprintf(((_tmp969=" %s %s %s",
_tag_dyneither(_tmp969,sizeof(char),10))),_tag_dyneither(_tmp968,sizeof(void*),3))))))))))))))))))));};};}
if(Cyc_ic_r){struct _handler_cons _tmp321;_push_handler(& _tmp321);{int _tmp323=0;
if(setjmp(_tmp321.handler))_tmp323=1;if(!_tmp323){Cyc_ccargs=((struct Cyc_List_List*(*)(
int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,
Cyc_ccargs);Cyc_libargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),
struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{struct Cyc_List_List*
_tmp324=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(stdlib,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));if(!_tmp30B){struct
Cyc_List_List*_tmp96A;_tmp324=((_tmp96A=_cycalloc(sizeof(*_tmp96A)),((_tmp96A->hd=
Cyc_final_strptr,((_tmp96A->tl=_tmp324,_tmp96A))))));}{struct Cyc_Interface_I*
_tmp326=((struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*get)(struct
_dyneither_ptr*),struct Cyc_List_List*la,struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(
Cyc_read_cycio,_tmp324,_tmp324);if(_tmp326 == 0){{const char*_tmp96D;void*_tmp96C;(
_tmp96C=0,Cyc_fprintf(Cyc_stderr,((_tmp96D="Error: interfaces incompatible\n",
_tag_dyneither(_tmp96D,sizeof(char),32))),_tag_dyneither(_tmp96C,sizeof(void*),0)));}
Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp329=1;_npop_handler(1);return
_tmp329;};}if(_tmp30B){if(Cyc_output_file != 0){const char*_tmp971;void*_tmp970[1];
struct Cyc_String_pa_struct _tmp96F;struct _dyneither_ptr _tmp32A=(_tmp96F.tag=0,((
_tmp96F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Filename_chop_extension(*((
struct _dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmp970[0]=& _tmp96F,Cyc_aprintf(((
_tmp971="%s.cycio",_tag_dyneither(_tmp971,sizeof(char),9))),_tag_dyneither(
_tmp970,sizeof(void*),1)))))));const char*_tmp973;const char*_tmp972;struct Cyc___cycFILE*
_tmp32B=Cyc_try_file_open((struct _dyneither_ptr)_tmp32A,((_tmp972="wb",
_tag_dyneither(_tmp972,sizeof(char),3))),((_tmp973="interface object file",
_tag_dyneither(_tmp973,sizeof(char),22))));if(_tmp32B == 0){Cyc_compile_failure=1;
Cyc_remove_cfiles();{int _tmp32C=1;_npop_handler(1);return _tmp32C;};}Cyc_Interface_save((
struct Cyc_Interface_I*)_tmp326,(struct Cyc___cycFILE*)_tmp32B);Cyc_file_close((
struct Cyc___cycFILE*)_tmp32B);}}else{const char*_tmp978;const char*_tmp977;struct
_tuple18*_tmp976;if(!Cyc_Interface_is_subinterface(Cyc_Interface_empty(),(struct
Cyc_Interface_I*)_tmp326,((_tmp976=_cycalloc(sizeof(*_tmp976)),((_tmp976->f1=((
_tmp978="empty interface",_tag_dyneither(_tmp978,sizeof(char),16))),((_tmp976->f2=((
_tmp977="global interface",_tag_dyneither(_tmp977,sizeof(char),17))),_tmp976)))))))){{
const char*_tmp97B;void*_tmp97A;(_tmp97A=0,Cyc_fprintf(Cyc_stderr,((_tmp97B="Error: some objects are still undefined\n",
_tag_dyneither(_tmp97B,sizeof(char),41))),_tag_dyneither(_tmp97A,sizeof(void*),0)));}
Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp337=1;_npop_handler(1);return
_tmp337;};}}};};;_pop_handler();}else{void*_tmp322=(void*)_exn_thrown;void*
_tmp339=_tmp322;_LL3B:;_LL3C:{void*_tmp33A=_tmp339;struct _dyneither_ptr _tmp33C;
struct _dyneither_ptr _tmp33E;struct _dyneither_ptr _tmp341;_LL40: {struct Cyc_Core_Failure_struct*
_tmp33B=(struct Cyc_Core_Failure_struct*)_tmp33A;if(_tmp33B->tag != Cyc_Core_Failure)
goto _LL42;else{_tmp33C=_tmp33B->f1;}}_LL41:{const char*_tmp97F;void*_tmp97E[1];
struct Cyc_String_pa_struct _tmp97D;(_tmp97D.tag=0,((_tmp97D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp33C),((_tmp97E[0]=& _tmp97D,Cyc_fprintf(
Cyc_stderr,((_tmp97F="Exception Core::Failure %s\n",_tag_dyneither(_tmp97F,
sizeof(char),28))),_tag_dyneither(_tmp97E,sizeof(void*),1)))))));}goto _LL3F;
_LL42: {struct Cyc_Core_Impossible_struct*_tmp33D=(struct Cyc_Core_Impossible_struct*)
_tmp33A;if(_tmp33D->tag != Cyc_Core_Impossible)goto _LL44;else{_tmp33E=_tmp33D->f1;}}
_LL43:{const char*_tmp983;void*_tmp982[1];struct Cyc_String_pa_struct _tmp981;(
_tmp981.tag=0,((_tmp981.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp33E),((
_tmp982[0]=& _tmp981,Cyc_fprintf(Cyc_stderr,((_tmp983="Exception Core::Impossible %s\n",
_tag_dyneither(_tmp983,sizeof(char),31))),_tag_dyneither(_tmp982,sizeof(void*),1)))))));}
goto _LL3F;_LL44: {struct Cyc_Dict_Absent_struct*_tmp33F=(struct Cyc_Dict_Absent_struct*)
_tmp33A;if(_tmp33F->tag != Cyc_Dict_Absent)goto _LL46;}_LL45:{const char*_tmp986;
void*_tmp985;(_tmp985=0,Cyc_fprintf(Cyc_stderr,((_tmp986="Exception Dict::Absent\n",
_tag_dyneither(_tmp986,sizeof(char),24))),_tag_dyneither(_tmp985,sizeof(void*),0)));}
goto _LL3F;_LL46: {struct Cyc_Core_Invalid_argument_struct*_tmp340=(struct Cyc_Core_Invalid_argument_struct*)
_tmp33A;if(_tmp340->tag != Cyc_Core_Invalid_argument)goto _LL48;else{_tmp341=
_tmp340->f1;}}_LL47:{const char*_tmp98A;void*_tmp989[1];struct Cyc_String_pa_struct
_tmp988;(_tmp988.tag=0,((_tmp988.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp341),((_tmp989[0]=& _tmp988,Cyc_fprintf(Cyc_stderr,((_tmp98A="Exception Core::Invalid_argument %s\n",
_tag_dyneither(_tmp98A,sizeof(char),37))),_tag_dyneither(_tmp989,sizeof(void*),1)))))));}
goto _LL3F;_LL48:;_LL49:{const char*_tmp98D;void*_tmp98C;(_tmp98C=0,Cyc_fprintf(
Cyc_stderr,((_tmp98D="Uncaught exception\n",_tag_dyneither(_tmp98D,sizeof(char),
20))),_tag_dyneither(_tmp98C,sizeof(void*),0)));}goto _LL3F;_LL3F:;}Cyc_compile_failure=
1;Cyc_remove_cfiles();{int _tmp34F=1;_npop_handler(0);return _tmp34F;};_LL3D:;
_LL3E:(void)_throw(_tmp339);_LL3A:;}};}{const char*_tmp98E;struct _dyneither_ptr
outfile_str=(_tmp98E="",_tag_dyneither(_tmp98E,sizeof(char),1));if(Cyc_output_file
!= 0){const char*_tmp992;void*_tmp991[1];struct Cyc_String_pa_struct _tmp990;
outfile_str=(struct _dyneither_ptr)((_tmp990.tag=0,((_tmp990.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(*((struct
_dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmp991[0]=& _tmp990,Cyc_aprintf(((
_tmp992=" -o %s",_tag_dyneither(_tmp992,sizeof(char),7))),_tag_dyneither(_tmp991,
sizeof(void*),1))))))));}{const char*_tmp99A;void*_tmp999[5];struct Cyc_String_pa_struct
_tmp998;struct Cyc_String_pa_struct _tmp997;struct Cyc_String_pa_struct _tmp996;
struct Cyc_String_pa_struct _tmp995;struct Cyc_String_pa_struct _tmp994;struct
_dyneither_ptr _tmp353=(_tmp994.tag=0,((_tmp994.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp30A),((_tmp995.tag=0,((_tmp995.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)stdlib_string),((_tmp996.tag=0,((_tmp996.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp309),((_tmp997.tag=0,((_tmp997.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)outfile_str),((_tmp998.tag=0,((
_tmp998.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)comp),((_tmp999[0]=&
_tmp998,((_tmp999[1]=& _tmp997,((_tmp999[2]=& _tmp996,((_tmp999[3]=& _tmp995,((
_tmp999[4]=& _tmp994,Cyc_aprintf(((_tmp99A="%s %s %s%s%s",_tag_dyneither(_tmp99A,
sizeof(char),13))),_tag_dyneither(_tmp999,sizeof(void*),5)))))))))))))))))))))))))))))));
if(Cyc_v_r){{const char*_tmp99E;void*_tmp99D[1];struct Cyc_String_pa_struct _tmp99C;(
_tmp99C.tag=0,((_tmp99C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp353),((
_tmp99D[0]=& _tmp99C,Cyc_fprintf(Cyc_stderr,((_tmp99E="%s\n",_tag_dyneither(
_tmp99E,sizeof(char),4))),_tag_dyneither(_tmp99D,sizeof(void*),1)))))));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);}if(system((const char*)_untag_dyneither_ptr(
_tmp353,sizeof(char),1))!= 0){{const char*_tmp9A1;void*_tmp9A0;(_tmp9A0=0,Cyc_fprintf(
Cyc_stderr,((_tmp9A1="Error: C compiler failed\n",_tag_dyneither(_tmp9A1,sizeof(
char),26))),_tag_dyneither(_tmp9A0,sizeof(void*),0)));}Cyc_compile_failure=1;Cyc_remove_cfiles();{
int _tmp359=1;_npop_handler(0);return _tmp359;};}Cyc_remove_cfiles();{int _tmp35A=
Cyc_compile_failure?1: 0;_npop_handler(0);return _tmp35A;};};};};};;_pop_region(r);};}
